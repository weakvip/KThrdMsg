#include "EpollMsg.h"

#if !defined(WINDOWS)
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>

/*****************************************************************/
#define AE_READABLE 1
#define AE_WRITEABLE 2
#define MAX_EVENTS 64
/*****************************************************************/
static int SetFdNonblocking(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	if (-1 == flag) {
		return -1;
	}

	flag |= O_NONBLOCK;

	if (-1 == fcntl(fd, F_SETFL, flag)) {
		return -1;
	}

	return 0;
}

static int AddEvent(int epfd, int fd, int mask)
{
	struct epoll_event ee;
	memset(&ee, 0x0, sizeof(ee));

	if (mask & AE_READABLE) {
		ee.events |= EPOLLIN | EPOLLET;
	}
	else {
		ee.events |= EPOLLET;
	}

	ee.data.fd = fd;

	if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ee)) return -1;

	return 0;
}
/*****************************************************************/
struct TThrdMsg {
	void* buf;
	size_t bufSize;
	int type;
	int id;
	void* data;
	size_t dataSize;
};
struct TThrdMsgEx {
	void* data;
};
/*****************************************************************/
static size_t MSG_API GetMsgDataSize(PThrdMsg msg)
{
	if (!msg) return 0;

	return msg->dataSize;
}
/*****************************************************************/
PThrdMsg MSG_API GetThrdMsg(void* buf, size_t bufSize, int type, int id, void* data, size_t dataSize)
{
	PThrdMsg pRet = 0;
	if (buf) {
		if (bufSize < sizeof(TThrdMsg)) {
			return 0;
		}

		pRet = (PThrdMsg)buf;
	}
	else {
		pRet = (PThrdMsg)malloc(sizeof(TThrdMsg));
	}

	if (!pRet) return 0;

	pRet->buf = buf;
	pRet->bufSize = bufSize;
	pRet->type = type;
	pRet->id = id;
	pRet->data = data;
	pRet->dataSize = dataSize;

	return pRet;
}

void MSG_API FreeThrdMsg(PThrdMsg msg)
{
	if (!msg) return;
	free(msg->data);
	if (!msg->buf) {
		free(msg);
	}
}

size_t MSG_API GetMsgSize(PThrdMsg msg)
{
	if (!msg) return 0;

	return sizeof(struct TThrdMsg);
}

int MSG_API GetMsgID(PThrdMsg msg)
{
	if (!msg) return 0;

	return msg->id;
}

void* MSG_API GetMsgData(PThrdMsg msg)
{
	if (!msg) return 0;

	return msg->data;
}
/*****************************************************************/
struct TThrdMsgMgr {
	int pipefd[2];
	int epfd;
	int msgCnt;
	//lock
};
/*****************************************************************/
PThrdMsgMgr MSG_API CreateThrdMsgMgr(void)
{
	PThrdMsgMgr pRet = (PThrdMsgMgr)malloc(sizeof(TThrdMsgMgr));
	if (!pRet) {
		return 0;
	}

	pRet->msgCnt = 0;

	if (pipe(pRet->pipefd) < 0) {
		return 0;
	}

	pRet->epfd = epoll_create(MAX_EVENTS);
	if (pRet->epfd < 0) {
		return 0;
	}

	//NON_BLOCK
	if (SetFdNonblocking(pRet->pipefd[0])) {
		return 0;
	}

	//ADD EVENT
	if (AddEvent(pRet->epfd, pRet->pipefd[0], AE_READABLE)) {
		return 0;
	}

	return pRet;
}

void MSG_API DestroyThrdMsgMgr(PThrdMsgMgr pObj)
{
	if (pObj) {
		close(pObj->epfd);
		close(pObj->pipefd[0]);
		close(pObj->pipefd[1]);

		pObj = 0;
	}
}

int MSG_API GetMsgCnt(PThrdMsgMgr pObj)
{
	if (!pObj) return 0;

	int cnt = __sync_fetch_and_add(&pObj->msgCnt, 0);

	return cnt;
}

int MSG_API AddMsg(PThrdMsgMgr obj, void* data, size_t size)
{
	if (!obj || !size) return -1;

	struct TThrdMsgEx msgEx = { data };
	int n = write(obj->pipefd[1], &msgEx, sizeof(msgEx));
	if (n > 0) {
		__sync_fetch_and_add(&obj->msgCnt, 1);
		return RESULT_MSG_SUCESS;
	}

	return -1;
}

void* MSG_API GetMsg(PThrdMsgMgr obj, int timeout)
{
	if (!obj) {
		return 0;
	}

	do {
		struct TThrdMsgEx buf;
		int n = read(obj->pipefd[0], &buf, sizeof(void*));
		if ( sizeof(void*)==n) {
			__sync_fetch_and_sub(&obj->msgCnt, 1);

			PThrdMsg pMsg = (PThrdMsg)buf.data;			
			return pMsg;
		}
		else if (n > 0) {
			//save to stack
			assert(0);
		}
	} while (0);

	struct epoll_event* e = calloc(MAX_EVENTS, sizeof(struct epoll_event));
	if (!e) {
		return 0;
	}

	int ret = epoll_wait(obj->epfd, e, MAX_EVENTS, timeout);

	free(e);

	if (ret > 0) {
		obj->msgCnt = 1;
		int i;
		for (i = 0; i < ret; ++i) {
			struct TThrdMsgEx buf;
			int n = read(obj->pipefd[0], &buf, sizeof(void*));
			if (sizeof(void*) == n) {
				__sync_fetch_and_add(&obj->msgCnt, 1);

				PThrdMsg pMsg = (PThrdMsg)buf.data;
				return pMsg;
			}
			else if (n > 0) {
				//save to stack
				assert(0);
			}
		}
	}

	return 0;
}
/*****************************************************************/
#endif
/*****************************************************************/