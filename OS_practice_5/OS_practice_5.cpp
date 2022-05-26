#include <thread>
#include <ctime>
#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <mutex>
#include <Windows.h>
#include <WinUser.h>

#define TIMEQUANTUM 1000

void keyscan();
void draw(short, short, char);
void draw(short, short, std::string);
void firstTask();
void secondTask();
void thirdTask();

struct data {
	unsigned long long i = 0;
	unsigned long long c = 0;
	short percent = 0;
};

bool firstFinished, secondFinished, thirdFinished, firstTimeOut, secondTimeOut, thirdTimeOut;
int priority[3];
bool paused[3];
data firstData, secondData, thirdData;
std::mutex patrol;

int main() {
	firstFinished = false, secondFinished = false, thirdFinished = false, paused[0] = false, paused[1] = false, paused[2] = false, firstTimeOut = false, secondTimeOut = false, thirdTimeOut = false;
	priority[0] = 1, priority[1] = 1, priority[2] = 1;
	CONSOLE_CURSOR_INFO inf;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &inf);
	inf.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &inf);
	std::thread keyscanner(keyscan);
	std::cout << " > 1st thread: Probability of rolling a five on a dice\n\n    Priority: |\n    Progress: __________ 0%\n\n   2nd thread: Probability of getting a royal flush in a four-player game\n\n    Priority: |\n    Progress: __________ 0%\n\n   3rd thread: Probability of getting tails 10 times in a row\n\n    Priority: |\n    Progress: __________ 0%\n\n" << char(24) << '/' << char(25) << "\n+/-\nspace";
	while (!(firstFinished and secondFinished and thirdFinished)) {
		long ts, ct = 0;
		if (!firstFinished and !paused[0] and priority[0]) {
			ts = clock();
			firstTimeOut = false;
			std::thread firstThread(firstTask);
			firstThread.detach();
			for (;;) {
				ct = clock();
				if (ct - ts > TIMEQUANTUM * priority[0]) {
					firstTimeOut = true;
					break;
				}
			}
		}
		if (!secondFinished and !paused[1] and priority[1]) {
			ts = clock();
			secondTimeOut = false;
			std::thread secondThread(secondTask);
			secondThread.detach();
			for (;;) {
				ct = clock();
				if (ct - ts > TIMEQUANTUM * priority[1]) {
					secondTimeOut = true;
					break;
				}
			}
		}
		if (!thirdFinished and !paused[2] and priority[2]) {
			ts = clock();
			thirdTimeOut = false;
			std::thread thirdThread(thirdTask);
			thirdThread.detach();
			for (;;) {
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
			draw(1, pos * 5, ' ');
			pos = (2 + pos) % 3;
			draw(1, pos * 5, '>');
			Sleep(200);
		}
		if (GetKeyState(VK_DOWN) & 32768) {
			draw(1, pos * 5, ' ');
			pos = (4 + pos) % 3;
			draw(1, pos * 5, '>');
			Sleep(200);
		}
		if (GetKeyState(VK_OEM_MINUS) & 32768) {
			if (priority[pos] > 0) {
				priority[pos] --;
				draw(14 + priority[pos], pos * 5 + 2, ' ');
			};
			Sleep(200);
		}
		if (GetKeyState(VK_OEM_PLUS) & 32768 and GetKeyState(VK_SHIFT) & 32768) {
			priority[pos] ++;
			draw(13 + priority[pos], pos * 5 + 2, 124);
			Sleep(200);
		}
		if (GetKeyState(VK_SPACE) & 32768) {
			paused[pos] = !paused[pos];
			if (paused[pos]) draw(4, pos * 5 + 1, "Paused                             ");
			else draw(4, pos * 5 + 1, "      ");
			Sleep(200);
		}
	}
}

void draw(short x, short y, char symbol) {
	COORD pos;
	pos.X = x, pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	std::cout << symbol;
}

void draw(short x, short y, std::string symbols) {
	COORD pos;
	pos.X = x, pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	std::cout << symbols;
}

void firstTask() {
	patrol.lock();
	srand(time(0));
	unsigned long long i = firstData.i;
	unsigned long long amount = firstData.c;
	draw(4, 1, "Rolling a dice 1.800.000.000 times");
	for (; (!firstTimeOut) and (!paused[0]) and (i < 1800000000);) {
		if (rand() % 6 + 1 == 5) amount++;
		i++;
		if (i % 18000000 == 0) {
			firstData.percent++;
			draw(25, 3, std::to_string(firstData.percent) + "%");
			if (firstData.percent % 10 == 0) draw(13 + firstData.percent / 10, 3, 219);
		}
	}
	firstData.i = i;
	firstData.c = amount;
	if (!paused[0]) draw(4, 1, "Waiting...                        ");
	if (i == 1800000000) {
		firstFinished = true;
		draw(4, 1, "This probability is " + std::to_string((double)amount / (double)i * 100) + "%");
	}
	patrol.unlock();
}

void secondTask() {
	patrol.lock();
	srand(time(0));
	unsigned long long i = secondData.i;
	unsigned long long amount = secondData.c;
	draw(4, 6, "Dealing cards 360.000.000 times");
	for (; (!secondTimeOut) and (!paused[1]) and (i < 360000000);) {
		short pool[13];
		for (short j = 0; j < 13;) {
			pool[j] = rand() % 52;
			bool flag = true;
			for (short k = 0; k < j; k++) {
				if (pool[j] == pool[k]) {
					flag = false;
				}
			}
			if (flag) j++;
		}
		short cards[4][5];
		for (short j = 0; j < 4; j++) for (short k = 0; k < 5; k++) cards[j][k] = false;
		if (7 < pool[0] % 13) cards[pool[0] / 13][pool[0] % 13 - 8] = true;
		if (7 < pool[4] % 13) cards[pool[4] / 13][pool[4] % 13 - 8] = true;
		if (7 < pool[8] % 13) cards[pool[8] / 13][pool[8] % 13 - 8] = true;
		if (7 < pool[9] % 13) cards[pool[9] / 13][pool[9] % 13 - 8] = true;
		if (7 < pool[10] % 13) cards[pool[10] / 13][pool[10] % 13 - 8] = true;
		if (7 < pool[11] % 13) cards[pool[11] / 13][pool[11] % 13 - 8] = true;
		if (7 < pool[12] % 13) cards[pool[12] / 13][pool[12] % 13 - 8] = true;
		for (short j = 0; j < 4; j++) {
			if (cards[j][0] and cards[j][1] and cards[j][2] and cards[j][3] and cards[j][4]) amount++;
		}
		i++;
		if (i % 3600000 == 0) {
			secondData.percent++;
			draw(25, 8, std::to_string(secondData.percent) + "%");
			if (secondData.percent % 10 == 0) draw(13 + secondData.percent / 10, 8, 219);
		}
	}
	secondData.i = i;
	secondData.c = amount;
	if (!paused[1]) draw(4, 6, "Waiting...                       ");
	if (i == 360000000) {
		secondFinished = true;
		draw(4, 6, "This probability is " + std::to_string((double)amount / (double)i * 100) + "%");
	}
	patrol.unlock();
}

void thirdTask() {
	patrol.lock();
	srand(time(0));
	unsigned long long i = thirdData.i;
	unsigned long long amount = thirdData.c;
	draw(4, 11, "Tossing a coin 10.000.000.000 times");
	for (; (!thirdTimeOut) and (!paused[2]) and (i < 1000000000);) {
		if (rand() % 2) if (rand() % 2) if (rand() % 2) if (rand() % 2) if (rand() % 2) if (rand() % 2) if (rand() % 2) if (rand() % 2) if (rand() % 2) if (rand() % 2) amount++;
		i++;
		if (i % 10000000 == 0) {
			thirdData.percent++;
			draw(25, 13, std::to_string(thirdData.percent) + "%");
			if (thirdData.percent % 10 == 0) draw(13 + thirdData.percent / 10, 13, 219);
		}
	}
	thirdData.i = i;
	thirdData.c = amount;
	if (!paused[1]) draw(4, 11, "Waiting...                         ");
	if (i == 1000000000) {
		secondFinished = true;
		draw(4, 11, "This probability is " + std::to_string((double)amount / (double)i * 100) + "%");
	}
	patrol.unlock();
}
