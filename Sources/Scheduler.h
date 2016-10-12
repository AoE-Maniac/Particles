#pragma once

struct Callback {
	void (*func)(Kore::u8*);
	Kore::u8 param[256];


	Callback() { }

	Callback(void(*f)(Kore::u8*), float p1, float p2, float p3, float p4, float p5, float p6) {
		func = f;
		((float*)param)[0] = p1;
		((float*)param)[1] = p2;
		((float*)param)[2] = p3;
		((float*)param)[3] = p4;
		((float*)param)[4] = p5;
		((float*)param)[5] = p6;
	}
};

void initScheduler();
void deleteScheduler();

void addSchedulerTask(Callback callback, float time);
float updateScheduler();