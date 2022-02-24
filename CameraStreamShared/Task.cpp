#include "Task.h"

TaskArgList::TaskArgList() {

}

TaskArgList::~TaskArgList() {
	for (void* argPtr : this->mArgPtrs) {
		if (argPtr) {
			free(argPtr);
		}
	}
	this->mArgPtrs.erase(
		this->mArgPtrs.begin(),
		this->mArgPtrs.end()
	);
}

//
//
//


Task::Task(TaskCallback callback) : mCallback(callback) { }

Task::~Task() {
	this->stop(5000, true);
}

void Task::addArg() { }

TaskArgList& Task::args() {
	return this->mArgs;
}

const TaskArgList& Task::args() const {
	return this->mArgs;
}

bool& Task::flag() {
	return this->mFlag;
}
const bool& Task::flag() const {
	return this->mFlag;
}

bool Task::running() {
	return this->mRunning;
}

bool Task::start(TaskCallback cb) {
	this->mCallback = cb;
	return this->start();
}

bool Task::start() {
	if (this->mRunning) {
		return true;
	}

	this->mFlag = true;

	this->mThread = std::thread(
		this->mCallback,
		std::ref(this->mFlag),
		std::cref(this->mArgs)
	);

	return (this->mRunning = true);
}

bool Task::stop(long long milliTimeout, bool detachOnTimeout) {
	if (!this->mRunning) {
		return true;
	}

	this->mFlag = false;

	if (milliTimeout > 0) {
		auto pre = std::chrono::high_resolution_clock::now();

		while (!this->mThread.joinable()) {
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pre).count() >= milliTimeout) {
				if (detachOnTimeout) {
					this->mThread.detach();
					break;
				}
				else {
					return false;
				}
			}
		}
	}
	else {
		while (!this->mThread.joinable());
	}

	this->mThread.join();
	this->mRunning = false;
	
	return true;
}

Task& Task::operator=(const Task& t) {
	this->mCallback = t.mCallback;
	return *this;
}