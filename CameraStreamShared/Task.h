#include <cstdarg>
#include <vector>
#include <thread>
#include <mutex>

#pragma once



class TaskArgList {
private:
	std::vector<void*> mArgPtrs;

public:
	TaskArgList();
	~TaskArgList();

	template<typename T>
	void add(const T& t) {
		T* tBuf = (T*)malloc(sizeof(T));
		*tBuf = t;
		mArgPtrs.push_back(tBuf);
	}

	template<typename T>
	T& at(size_t i) {
		return *(T*)this->mArgPtrs.at(i);
	}

	template<typename T>
	const T& at(size_t i) const {
		return *(const T*)this->mArgPtrs.at(i);
	}

	size_t size() const {
		return this->mArgPtrs.size();
	}
};


typedef void(*TaskCallback)(bool& flag, const TaskArgList& args);


class Task
{
private:
	std::thread mThread;
	TaskCallback mCallback = NULL;
	TaskArgList mArgs;
	bool mFlag = true, mRunning = false;

	void addArg();

	template<typename ArgType>
	void addArg(const ArgType& arg) {
		this->mArgs.add<ArgType>(arg);
	}

	template<typename ArgType, typename ... RestOfArgTypes>
	void addArg(const ArgType& arg, const RestOfArgTypes& ...rest) {
		this->mArgs.add<ArgType>(arg);
		this->addArg(rest...);
	}

public:
	Task(TaskCallback callback);

	template<typename ... AllOfArgTypes>
	Task(TaskCallback callback, const AllOfArgTypes& ...args) : Task(callback) {
		this->addArg(args...);
	}

	~Task();

	TaskArgList& args();
	const TaskArgList& args() const;

	bool& flag();
	const bool& flag() const;

	bool running();

	bool start();
	bool start(TaskCallback cb);
	
	bool stop(long long milliTimeout = -1, bool detachOnTimeout = false);
	
	Task& operator=(const Task& t);
};



class TaskPool : private std::vector<Task> {

};

