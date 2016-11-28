#include "IocpMsg.h"

#if defined(WINDOWS)

#include <Windows.h>
/*****************************************************************/

/*****************************************************************/
struct TThrdMsg{
	OVERLAPPED overlapped;
	void* buf;
	size_t bufSize;
	int type;
	int id;
	void* data;
	size_t dataSize;
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
struct TThrdMsgMgr{
	HANDLE iocp;
	long msgCnt;
};
/*****************************************************************/
PThrdMsgMgr MSG_API CreateThrdMsgMgr(void)
{
	PThrdMsgMgr pRet = (PThrdMsgMgr)malloc(sizeof(TThrdMsgMgr));
	if (!pRet) return 0;

	pRet->iocp = INVALID_HANDLE_VALUE;
	pRet->msgCnt = 0;
	pRet->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (INVALID_HANDLE_VALUE == pRet->iocp) return 0;

	return pRet;
}

void MSG_API DestroyThrdMsgMgr(PThrdMsgMgr pObj)
{
	if (pObj){
		CloseHandle(pObj->iocp);
		pObj = 0;
	}
}

int MSG_API GetMsgCnt(PThrdMsgMgr pObj)
{
	if (!pObj) return 0;

	long lCnt = InterlockedExchangeAdd(&pObj->msgCnt,0);

	return (int)lCnt;
}

int MSG_API AddMsg(PThrdMsgMgr obj, void* data, size_t size)
{
	if (!obj || INVALID_HANDLE_VALUE == obj->iocp || !data) return RESULT_MSG_FAILED;

	do{
		BOOL bOK = PostQueuedCompletionStatus(obj->iocp, (DWORD)size, 0, (LPOVERLAPPED)data);
		if (!bOK && ERROR_IO_PENDING == GetLastError()){
			return RESULT_MSG_FAILED;
		}
		InterlockedIncrement(&obj->msgCnt);
	} while (0);

	return RESULT_MSG_SUCESS;
}

void* MSG_API GetMsg(PThrdMsgMgr obj, int timeout)
{
	if (!obj || INVALID_HANDLE_VALUE == obj->iocp) return 0;

	do{
		DWORD dwNumberOfBytes = 0;
		ULONG_PTR ulpCompletionKey = 0;
		LPOVERLAPPED lpOverlapped = 0;
		DWORD dwTimeout = (DWORD)timeout;

		BOOL bOK = GetQueuedCompletionStatus(obj->iocp, &dwNumberOfBytes, &ulpCompletionKey, &lpOverlapped, dwTimeout);
		if (bOK){
			InterlockedDecrement(&obj->msgCnt);
			return (void*)lpOverlapped;
		}
	} while (0);	

	return 0;
}
/*****************************************************************/
#endif
/*****************************************************************/