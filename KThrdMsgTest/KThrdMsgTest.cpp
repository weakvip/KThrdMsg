// KThreadMsgTest.cpp : Defines the entry point for the console application.
//

#include <Inc/ThrdMsg/IThrdMsg.h>
#include <string>
#include <iostream>
#include <chrono>

#ifdef WINDOWS
#include <Windows.h>
#endif

int main()
{
	PThrdMsgMgr pMgr = CreateThrdMsgMgr();
	if (!pMgr) {
		std::cout << "create msgmgr error" << std::endl;
		return -1;
	}

	for (;;) {

		int cnt = 0;

		std::chrono::time_point<std::chrono::system_clock> p1, p2;
		p1 = std::chrono::system_clock::now();

		const int cnBufSize = 20;
		const int cnLoopCnt = 20;

		for (int i = 0; i < cnLoopCnt; ++i) {
			char* pbuf = (char*)malloc(cnBufSize);
			snprintf(pbuf, cnBufSize, "%s", "hello world");
			pbuf[cnBufSize - 1] = '\0';
			PThrdMsg pMsg = GetThrdMsg(0, 0, 0, 0, pbuf, cnBufSize);
			if (pMsg) {
				int n = AddMsg(pMgr, pMsg, sizeof(pMsg));
				assert(0 == n);
				++cnt;
			}
		}

		std::cout << "add cnt:" << cnt << std::endl;
		cnt = 0;

		for (;;) {
			PThrdMsg pMsgR = (PThrdMsg)GetMsg(pMgr, 0);
			if (pMsgR) {
				++cnt;
				std::cout << "msg[" << (char*)GetMsgData(pMsgR) << "]" << std::endl;
				FreeThrdMsg(pMsgR);
			}
			else {
				break;
			}
		};

		std::cout << "get cnt:" << cnt << std::endl;

		p2 = std::chrono::system_clock::now();

		std::cout << "last:" << (p2.time_since_epoch().count() - p1.time_since_epoch().count()) << std::endl;

#ifdef WINDOWS

		Sleep(1000*2);
#else
		usleep(1000 * 1000*2);
#endif

	}

	DestroyThrdMsgMgr(pMgr);	

	std::cout << "game over" << std::endl;

#ifdef WINDOWS
	system("pause");
#else
	usleep(1000 * 1000*2);
#endif

    return 0;
}

