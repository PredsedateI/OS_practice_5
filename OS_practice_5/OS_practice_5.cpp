#include <thread>
#include <ctime>
#include <iostream>
#include <Windows.h>
#include <WinUser.h>

#define TIMEQUANTUM 1000

void keyscan();
void firstTask();
void secondTask();
void thirdTask();

bool firstFinished, secondFinished, thirdFinished, firstTimeOut, secondTimeOut, thirdTimeOut;
int priority[3];

int main() {
	firstFinished = false, secondFinished = false, thirdFinished = false, firstTimeOut = false, secondTimeOut = false, thirdTimeOut = false;
	priority[0] = 1, priority[1] = 1, priority[2] = 1;
	std::thread keyscanner(keyscan);
	while (!(firstFinished and secondFinished and thirdFinished)) {
		long ts, ct;
		std::thread firstThread, secondThread, thirdThread;
		if (!firstFinished && priority[0]) {
			ts = clock();
			firstTimeOut = false;
			firstThread = std::thread(firstTask);
			firstThread.detach();
			while (true) {
				ct = clock();
				if (ct - ts > TIMEQUANTUM * priority[0]) {
					firstTimeOut = true;
					break;
				}
			}
		}
		if (!secondFinished && priority[1]) {
			ts = clock();
			secondTimeOut = false;
			secondThread = std::thread(secondTask);
			secondThread.detach();
			while (true) {
				ct = clock();
				if (ct - ts > TIMEQUANTUM * priority[1]) {
					secondTimeOut = true;
					break;
				}
			}
		}
		if (!thirdFinished && priority[2]) {
			ts = clock();
			thirdTimeOut = false;
			thirdThread = std::thread(thirdTask);
			thirdThread.detach();
			while (true) {
				ct = clock();
				if (ct - ts > TIMEQUANTUM * priority[2]) {
					thirdTimeOut = true;
					break;
				}
			}
		}
	}
	return 0;
}

void keyscan() {
	short pos = 0;
	for (;;) {
		if (GetKeyState(VK_UP) & 32768) { 
			pos = (2 + pos) % 3;
			Sleep(300);
		};
		if (GetKeyState(VK_DOWN) & 32768) {
			pos = (4 + pos) % 3;
			Sleep(300);
		};
		if (GetKeyState(VK_OEM_MINUS) & 32768) {
			priority[pos] --;
			Sleep(300);
		};
		if (GetKeyState(VK_OEM_PLUS) & 32768 && GetKeyState(VK_SHIFT) & 32768) {
			priority[pos] ++;
			Sleep(300);
		};
		//std::cout << pos << ' ' << priority[0] << ' ' << priority[1] << ' ' << priority[2] << '\n';
	}
}

void firstTask() {
	for (;;) {
		if (firstTimeOut) {
			break;
		}
		std::cout << 1;
	}
}

void secondTask() {
	for (;;) {
		if (secondTimeOut) {
			break;
		}
		std::cout << 2;
	}
}

void thirdTask() {
	for (;;) {
		if (thirdTimeOut) {
			break;
		}
		std::cout << 3;
	}
}