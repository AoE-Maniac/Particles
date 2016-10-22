#pragma once

struct Task {
	float time;
	void (*func)(Kore::u8*);
	Kore::u8 param[256];


	Task() { }

	Task(float t, void(*f)(Kore::u8*), float p1, float p2, float p3, float p4, float p5, float p6) {
		time = t;
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

void addSchedulerTask(Task task);
float updateScheduler();