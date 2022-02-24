#include "Logger.h"

const bool Logger::__LOGGER_IS_INITD__ = Logger::__LOGGER_INIT__();

const LoggerPrefixes Logger::DEFAULT_LOGGER_PREFIXES = {
	"[ " ANSI_FORE_GREEN_BRIGHT "\xFB" ANSI_RESET " ] : ",
	"[ " ANSI_FORE_BLUE_BRIGHT "i" ANSI_RESET " ] : ",
	"[ " ANSI_FORE_YELLOW "?" ANSI_RESET " ] : ",
	"[ " ANSI_FORE_RED_BRIGHT "!" ANSI_RESET " ] : ",
	"[" ANSI_FORE_MAGENTA_BRIGHT "%s" ANSI_RESET "@" ANSI_FORE_CYAN_BRIGHT "0x%p" ANSI_RESET "] - "
};

//
// (De)Constructors
//

Logger::Logger(
	const char* logFilePath,
	const LoggerPrefixes* prefixes
) {
	if (logFilePath) {
		FILE* logFileOutputStream;
		files::path logFilePathType(logFilePath);

		if (!files::exists(logFilePathType) && logFilePathType.has_parent_path()) {
			files::create_directories(logFilePathType.parent_path());
		}

		logFileOutputStream = fopen(logFilePath, "a");

		if (logFileOutputStream) {
			this->_logOutputStreams.push_back(logFileOutputStream);
		}
	}

	if (prefixes) {
		this->_prefixes = *prefixes;
	}
}

Logger::Logger(
	const char* logFilePath
) : Logger(logFilePath, NULL) {

}

Logger::Logger() : Logger(NULL, NULL) { }

Logger::~Logger() {
	for (FILE* logOutputStream : this->_logOutputStreams) {
		if (logOutputStream != stdout && logOutputStream != stderr) {
			fclose(logOutputStream);
		}
	}
}

//
//
//

void Logger::logChar(const char c) {
	for (FILE* logOutputStream : this->_logOutputStreams) {
		fputc(c, logOutputStream);
	}
}

void Logger::logString(const char* cstr) {
	bool ansiStyleFlag;

	for (FILE* logOutputStream : this->_logOutputStreams) {
		if (logOutputStream == NULL) {
			continue;
		}

		if (logOutputStream == stdout || logOutputStream == stderr) {
			fputs(cstr, logOutputStream);
		}
		else {
			ansiStyleFlag = false;
			for (size_t i = 0; cstr[i]; i++) {
				switch (cstr[i])
				{
				case '\u001b':
					ansiStyleFlag = true;
					continue;

				case 'm':
					if (ansiStyleFlag) {
						ansiStyleFlag = false;
						continue;
					}

				default:
					if (!ansiStyleFlag) {
						fputc(cstr[i], logOutputStream);
					}
				}
			}
		}
	}
}

void Logger::logVarFormat(const char* format, va_list varArgs) {
	int writtenLen;
	
	while ((writtenLen = vsnprintf(this->_formatBuffer.data(), this->_formatBuffer.size(), format, varArgs))
		>= (int)this->_formatBuffer.size()) {
		this->_formatBuffer.resize((size_t)writtenLen + 1);
	}

	this->logString(this->_formatBuffer.data());
}

void Logger::logFormat(const char* format, ...) {
	va_list varArgs;
	va_start(varArgs, format);
	this->logVarFormat(format, varArgs);
	va_end(varArgs);
}

void Logger::logPrefix(const char* prefix, const char* format, va_list varArgs) {
	if (this->_baseTypeName) {
		this->logFormat(this->_prefixes.instanceInfoPrefix, this->_baseTypeName, (void*)this);
	}
	this->logString(prefix);
	this->logVarFormat(format, varArgs);
	this->logChar('\n');
}


//
//
//

Logger& Logger::addOutputStream(FILE* logOutputStreamToAdd) {
	for (FILE* logOutputStream : this->_logOutputStreams) {
		if (logOutputStream == logOutputStreamToAdd) {
			goto out;
		}
	}

	this->_logOutputStreams.push_back(logOutputStreamToAdd);

out:
	return *this;
}

void Logger::logSuccess(const char* format, ...) {
	va_list varArgs;
	va_start(varArgs, format);
	this->logPrefix(this->DEFAULT_LOGGER_PREFIXES.successPrefix, format, varArgs);
	va_end(varArgs);
}

void Logger::logInformation(const char* format, ...) {
	va_list varArgs;
	va_start(varArgs, format);
	this->logPrefix(this->DEFAULT_LOGGER_PREFIXES.informationPrefix, format, varArgs);
	va_end(varArgs);
}

void Logger::logWarning(const char* format, ...) {
	va_list varArgs;
	va_start(varArgs, format);
	this->logPrefix(this->DEFAULT_LOGGER_PREFIXES.warningPrefix, format, varArgs);
	va_end(varArgs);
}

void Logger::logError(const char* format, ...) {
	va_list varArgs;
	va_start(varArgs, format);
	this->logPrefix(this->DEFAULT_LOGGER_PREFIXES.errorPrefix, format, varArgs);
	va_end(varArgs);
}