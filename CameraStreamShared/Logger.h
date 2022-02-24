#ifndef _LOGGER_
#define _LOGGER_

#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
#	include <Windows.h>
#endif

#include "Ansi.h"

#include <iostream>
#include <typeinfo>
#include <fstream>
#include <cstdarg>
#include <vector>

#include <filesystem>

#pragma once

namespace files = std::filesystem;

typedef struct {
	const char* successPrefix;
	const char* informationPrefix;
	const char* warningPrefix;
	const char* errorPrefix;
	const char* instanceInfoPrefix;
} LoggerPrefixes, *pLoggerPrefixes;


class Logger {
private:
	static inline bool __LOGGER_INIT__() {
#ifdef _WIN32
		HANDLE consoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD consoleCurrentMode;

		if (!GetConsoleMode(consoleOutputHandle, &consoleCurrentMode) 
			|| !SetConsoleMode(consoleOutputHandle, consoleCurrentMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
			throw std::runtime_error("Unable To Enable Windows Console Ansi Support");
		}
#endif

		return true;
	}

	static const bool __LOGGER_IS_INITD__;

	static const LoggerPrefixes DEFAULT_LOGGER_PREFIXES;

private:
	std::vector<FILE*> _logOutputStreams = { stdout };

	const char* _baseTypeName = NULL;
	
	LoggerPrefixes _prefixes = Logger::DEFAULT_LOGGER_PREFIXES;

	std::vector<char> _formatBuffer;

public:

	Logger(
		const char* logFilePath,
		const LoggerPrefixes* prefixes
	);

	template<typename BaseTypeInstance>
	Logger(
		const BaseTypeInstance* pInstance,
		const char* logFilePath,
		const LoggerPrefixes* prefixes
	) : Logger(logFilePath, prefixes) {
		this->_baseTypeName = typeid(BaseTypeInstance).name();
	}

	Logger(
		const char* logFilePath
	);

	template<typename BaseTypeInstance>
	Logger(
		const BaseTypeInstance* pInstance,
		const char* logFilePath
	) : Logger(logFilePath, NULL) {
		this->_baseTypeName = typeid(BaseTypeInstance).name();
	}

	Logger();

	template<typename BaseTypeInstance>
	Logger(
		const BaseTypeInstance* pInstance
	) : Logger(pInstance, NULL, NULL) { }

	Logger(const Logger& lref) = delete;

	~Logger();

protected:

	void logChar(const char c);
	void logString(const char* cstr);
	void logVarFormat(const char* format, va_list varArgs);
	void logFormat(const char* format, ...);

	void logPrefix(const char* prefix, const char* format, va_list varArgs);

public:

	Logger& addOutputStream(FILE* logOutputStream);

	void logSuccess(const char* format, ...);
	void logInformation(const char* format, ...);
	void logWarning(const char* format, ...);
	void logError(const char* format, ...);

	void logInstanceState() = delete;

public:

	Logger& operator=(const Logger& lref) = delete;

};

#endif