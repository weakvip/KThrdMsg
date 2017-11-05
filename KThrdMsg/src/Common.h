#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif

struct thrd_msg_mgr_t {
#ifdef _WIN32
	HANDLE iocp;
#else
	int pipefd[2];
	int epfd;
	//lock
#endif
	int msgCnt;
};
