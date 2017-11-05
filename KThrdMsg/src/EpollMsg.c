#include "EpollMsg.h"

#ifdef __linux__
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define AE_READABLE 1
#define AE_WRITEABLE 2
#define MAX_EVENTS 64

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

typedef struct thrd_msg_inner_s {
	size_t size;
	void* data;	
}thrd_msg_inner_t;

thrd_msg_mgr_p MSG_API CreateThrdMsgMgr(void)
{
	thrd_msg_mgr_p pRet = (thrd_msg_mgr_p)malloc(sizeof(thrd_msg_mgr_t));
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

void MSG_API DestroyThrdMsgMgr(thrd_msg_mgr_p* ppObj)
{
	if (*ppObj) {
		close((*ppObj)->epfd);
		close((*ppObj)->pipefd[0]);
		close((*ppObj)->pipefd[1]);
		free(*ppObj);
		ppObj = 0;
	}
}

int MSG_API GetMsgCnt(thrd_msg_mgr_p obj)
{
	if (!obj) return 0;

	int cnt = __sync_fetch_and_add(&obj->msgCnt, 0);

	return cnt;
}

int MSG_API PushMsg(thrd_msg_mgr_p obj, void* data, int size)
{
	if (!obj) return -1;

	thrd_msg_inner_t msg = { size, data };
	int n = write(obj->pipefd[1], &msg, sizeof(msg));
	if (n <= 0) {
		return -1;
	}
	__sync_fetch_and_add(&obj->msgCnt, 1);
	return 0;
}

int MSG_API PopMsg(thrd_msg_mgr_p obj, int timeout, void** data, int* size)
{
	if (!obj || !data ||!size) {
		return -1;
	}

	do {
		thrd_msg_inner_t msg;
		int n = read(obj->pipefd[0], &msg, sizeof(msg));
		if (n>0) {
			__sync_fetch_and_sub(&obj->msgCnt, 1);
			*size = msg.size;
			*data = msg.data;
			return 0;
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
			thrd_msg_inner_t msg;
			int n = read(obj->pipefd[0], &msg, sizeof(msg));
			if (n <= 0) {
				return -1;
			}
			__sync_fetch_and_sub(&obj->msgCnt, 1);
			*size = msg.size;
			*data = msg.data;
			return 0;
		}
	}

	return -1;
}

#endif

