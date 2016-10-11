#pragma once

struct Callback {
	void (*func)(Kore::u8*);
	Kore::u8 param[256];


	Callback() { }

	Callback(void(*f)(Kore::u8*), float p1, float p2, float p3, float p4, float p5, float p6) {
		func = f;
		param[0] = p1;
		param[4] = p2;
		param[8] = p3;
		param[12] = p4;
		param[16] = p5;
		param[20] = p6;
	}
};

void initScheduler();
void deleteScheduler();

void addSchedulerTask(Callback callback, float time);
float updateScheduler();