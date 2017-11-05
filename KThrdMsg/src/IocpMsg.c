#include "IocpMsg.h"

#ifdef _WIN32

thrd_msg_mgr_p MSG_API CreateThrdMsgMgr(void)
{
	thrd_msg_mgr_p pRet = (thrd_msg_mgr_p)malloc(sizeof(thrd_msg_mgr_t));
	if (!pRet) return 0;

	pRet->msgCnt = 0;
	pRet->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (INVALID_HANDLE_VALUE == pRet->iocp) return 0;

	return pRet;
}

void MSG_API DestroyThrdMsgMgr(thrd_msg_mgr_p* ppObj)
{
	if (ppObj && *ppObj){
		CloseHandle((*ppObj)->iocp);
		free(*ppObj);
		ppObj = 0;
	}	
}

int MSG_API GetMsgCnt(thrd_msg_mgr_p obj)
{
	if (!obj) return 0;

	long lCnt = InterlockedExchangeAdd(&obj->msgCnt,0);

	return (int)lCnt;
}

int MSG_API PushMsg(thrd_msg_mgr_p obj, void* data, int size)
{
	if (!obj || INVALID_HANDLE_VALUE == obj->iocp) return -1;

	BOOL bOK = PostQueuedCompletionStatus(obj->iocp, (DWORD)size, 0, (LPOVERLAPPED)data);
	if (!bOK && ERROR_IO_PENDING != GetLastError()) {
		return -1;
	}
	InterlockedIncrement(&obj->msgCnt);

	return 0;
}

int MSG_API PopMsg(thrd_msg_mgr_p obj, int timeout, void** data, int* size)
{
	if (!obj ||!size || INVALID_HANDLE_VALUE == obj->iocp) return -1;

	*data = 0;
	*size = 0;

	DWORD dwNumberOfBytes = 0;
	ULONG_PTR ulpCompletionKey = 0;
	LPOVERLAPPED lpOverlapped = 0;
	DWORD dwTimeout = (DWORD)timeout;

	BOOL bOK = GetQueuedCompletionStatus(obj->iocp, &dwNumberOfBytes, &ulpCompletionKey, &lpOverlapped, dwTimeout);
	if (!bOK) return -1;

	*data = lpOverlapped;
	*size = dwNumberOfBytes;
	InterlockedDecrement(&obj->msgCnt);

	return 0;
}

#endif
