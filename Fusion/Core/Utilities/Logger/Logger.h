#pragma once

class Logger
{
public:
	enum class Severity
	{
		INFO = 0,
		WARN = (INFO + 1),
		ERR =  (WARN + 1)
	};

public:
	/* Log a string to console with custom severity level */
	static void Log(Severity severity, const char* message, ...);

private:
	static inline const char* SeverityToString(Severity severity);
	static inline void SetSeverityConsoleColor(Severity severity);
};

#define LOG_INFO(format, ...) Logger::Log(Logger::Severity::INFO, format, __VA_ARGS__)
#define LOG_WARN(format, ...) Logger::Log(Logger::Severity::WARN, format, __VA_ARGS__)
#define LOG_ERR(format, ...) Logger::Log(Logger::Severity::ERR, format, __VA_ARGS__)