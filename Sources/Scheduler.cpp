#include "pch.h"

#include "Scheduler.h"

#include <cassert>
#include <Kore/System.h>

using namespace Kore;

namespace {
	const int MAX_TASKS = 500;

	double startTime;
	double time;

	int currTaskCount;
	Task* currTaskList;

	int newTaskCount;
	Task* newTaskList;

	void insertSchedulerTask(Task task) {
		assert(currTaskCount < MAX_TASKS);

		if (currTaskCount < MAX_TASKS) {
			for (int i = currTaskCount - 1; i >= -1; --i) {
				if (i != -1 && currTaskList[i].time > time) {
					currTaskList[i + 1] = currTaskList[i];
				}
				else {
					currTaskList[i + 1] = task;
					break;
				}
			}

			++currTaskCount;
		}
	}
}

void initScheduler() {
	startTime = System::time();
	time = 0;

	currTaskCount = 0;
	currTaskList = new Task[MAX_TASKS];
	newTaskCount = 0;
	newTaskList = new Task[MAX_TASKS];
}

void deleteScheduler() {
	delete[] currTaskList;
	delete[] newTaskList;
}

void addSchedulerTask(Task task) {
	assert(newTaskCount < MAX_TASKS);

	if (newTaskCount < MAX_TASKS) {
		newTaskList[newTaskCount++] = task;
	}
}

float updateScheduler() {
	double t = System::time() - startTime;
	double deltaT = t - time;
	time = t;

	for (int i = 0; i < newTaskCount; ++i) {
		insertSchedulerTask(newTaskList[i]);
	}
	newTaskCount = 0;

	int toDelete = 0;
	for (int i = 0; i < currTaskCount; ++i) {
		if (currTaskList[i].time <= time) {
			currTaskList[i].func(currTaskList[i].param);
			++toDelete;
		}
		else {
			if (toDelete == 0) break;

			currTaskList[i - toDelete] = currTaskList[i];
		}
	}
	currTaskCount -= toDelete;




	return deltaT;
}