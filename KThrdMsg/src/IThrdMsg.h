//线程间消息通信接口
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RESULT_MSG_SUCESS 0
#define RESULT_MSG_FAILED -1

#if defined(_WIN32) || defined(_WIN64)
#ifndef WINDOWS
#define WINDOWS
#endif
#endif

#if defined(WINDOWS)
#define MSG_API __stdcall
#else
#define MSG_API
#endif

typedef struct TThrdMsg TThrdMsg, *PThrdMsg;

extern PThrdMsg MSG_API GetThrdMsg(void* buf, size_t bufSize, int type, int id, void* data, size_t dataSize);

extern void MSG_API FreeThrdMsg(PThrdMsg);

extern size_t MSG_API GetMsgSize(PThrdMsg msg);

extern int MSG_API GetMsgType(PThrdMsg);

extern int MSG_API GetMsgID(PThrdMsg);

extern void* MSG_API GetMsgData(PThrdMsg);


typedef struct TThrdMsgMgr TThrdMsgMgr, *PThrdMsgMgr;

extern  PThrdMsgMgr MSG_API CreateThrdMsgMgr(void);

extern  void MSG_API DestroyThrdMsgMgr(PThrdMsgMgr);

extern  int MSG_API GetMsgCnt(PThrdMsgMgr pObj);

extern  int MSG_API AddMsg(PThrdMsgMgr obj, void* data, size_t size);

extern  void* MSG_API GetMsg(PThrdMsgMgr obj, int timeout);

#ifdef __cplusplus
}
#endif
