//interface for message passing between threads freelock
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define MSG_API __stdcall
#else
#define MSG_API
#endif
	//declaration
	typedef struct thrd_msg_mgr_t thrd_msg_mgr_t, *thrd_msg_mgr_p;
	//create a thread message manager object
	extern  thrd_msg_mgr_p MSG_API CreateThrdMsgMgr(void);
	//destroy a thread message manager object
	extern  void MSG_API DestroyThrdMsgMgr(thrd_msg_mgr_p* ppObj);
	//retrieve message count from a thread message manager object
	extern  int MSG_API GetMsgCnt(thrd_msg_mgr_p obj);
	//push data into a thread message manager object, if succeed return 0, otherwise -1
	extern  int MSG_API PushMsg(thrd_msg_mgr_p obj, void* data, int size);
	//pop data from a thread message manager object, if succeed return 0, otherwise -1
	extern  int MSG_API PopMsg(thrd_msg_mgr_p obj, int timeout, void** data, int* size);
#ifdef __cplusplus
}
#endif
