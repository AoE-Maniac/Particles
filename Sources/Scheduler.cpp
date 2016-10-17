#include "pch.h"

#include "Scheduler.h"

#include <cassert>
#include <Kore/System.h>

using namespace Kore;

namespace {
	const int MAX_TASKS = 500;

	double startTime;
	double time;

	int currTasks;
	float* taskTimes;
	Callback* taskCallbacks;
}

void initScheduler() {
	startTime = System::time();
	time = 0;

	currTasks = 0;
	taskTimes = new float[MAX_TASKS];
	taskCallbacks = new Callback[MAX_TASKS];
}

void deleteScheduler() {
	delete[] taskTimes;
	delete[] taskCallbacks;
}

void addSchedulerTask(Callback callback, float time) {
	assert(currTasks < MAX_TASKS);

	if (currTasks < MAX_TASKS) {
		for (int i = currTasks - 1; i >= -1; --i) {
			if (i != -1 && taskTimes[i] > time) {
				taskTimes[i + 1] = taskTimes[i];
				taskCallbacks[i + 1] = taskCallbacks[i];
			}
			else {
				taskTimes[i + 1] = time;
				taskCallbacks[i + 1] = callback;
				break;
			}
		}

		++currTasks;
	}
}

float updateScheduler() {
	double t = System::time() - startTime;
	double deltaT = t - time;
	time = t;

	int toDelete = 0;
	for (int i = 0; i < currTasks; ++i) {
		if (taskTimes[i] <= time) {
			taskCallbacks[i].func(taskCallbacks[i].param);
			++toDelete;
		}
		else {
			if (toDelete == 0) break;

			taskTimes[i - toDelete] = taskTimes[i];
			taskCallbacks[i - toDelete] = taskCallbacks[i];
		}
	}

	currTasks -= toDelete;

	return deltaT;
}