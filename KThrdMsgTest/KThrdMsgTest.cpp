// KThreadMsgTest.cpp : Defines the entry point for the console application.
//
#include <assert.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <Inc/KThrdMsg/IThrdMsg.h>

int main()
{
	thrd_msg_mgr_p pMgr = CreateThrdMsgMgr();
	if (!pMgr) {
		std::cout << "create msgmgr error" << std::endl;
	}
	else {
		std::chrono::time_point<std::chrono::system_clock> p1, p2;
		const int cnBufSize = 20;
		const int cnLoopCnt = 10000;

		for (;;) {
			int cnt = 0;
			p1 = std::chrono::system_clock::now();

			for (int i = 0; i < cnLoopCnt; ++i) {
				char* buf = (char*)malloc(cnBufSize);
				if (buf) {
					snprintf(buf, cnBufSize, "%s", "hello world");
					*(buf + cnBufSize - 1) = '\0';
				}
				int n = PushMsg(pMgr, buf, cnBufSize);
				assert(0 == n);
				++cnt;
			}

			std::cout << "push cnt:" << cnt << std::endl;
			cnt = 0;

			for (;;) {
				char* buf = 0;
				int size = 0;
				int num = 0;
				if (PopMsg(pMgr, 0, (void**)&buf, &size)) {
					break;
				}
				if (buf && cnBufSize==size) {
					if (1 == ++cnt) {
						std::cout << "msg[" << buf << "]" << std::endl;
					}					
					free(buf);
				}
			};

			std::cout << "pop cnt:" << cnt << std::endl;

			p2 = std::chrono::system_clock::now();

			std::cout << "cost:" << (p2.time_since_epoch().count() - p1.time_since_epoch().count()) << std::endl;

			std::this_thread::sleep_for(std::chrono::seconds(2));
		}

		DestroyThrdMsgMgr(&pMgr);
	}

	std::cout << "game over" << std::endl;
#ifdef _WIN32
	system("pause");
#else
	std::cout << "press any key to continue...\n" << std::endl;
	getchar();
#endif

    return 0;
}
