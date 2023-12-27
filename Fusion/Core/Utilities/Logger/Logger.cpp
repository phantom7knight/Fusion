#include "Logger.h"
#include "../../CorePCH.hpp"

void Logger::Log(Severity severity, const char* message, ...)
{
	SetSeverityConsoleColor(severity);

	// Access the variadic arguments using va_list
	va_list args;
	va_start(args, message);

	std::string retMessage;
	size_t len = vsnprintf(0, 0, message, args);
	retMessage.resize(len + 1);  // need space for NUL
	vsnprintf(&retMessage[0], len + 1, message, args);
	retMessage.resize(len);  // remove the NUL

	// Clean up the va_list
	va_end(args);
	
	const std::string finalFullMessage = SeverityToString(severity) + retMessage + "\n";
	printf(finalFullMessage.c_str());
}

const char* Logger::SeverityToString(Severity severity)
{
	switch (severity)
	{
	case Severity::INFO:
		return "[INFO] ";
	case Severity::WARN:
		return "[WARN] ";
	case Severity::ERR:
		return "[ERR] ";
	default:
		return "[INFO] ";
	}
}

inline void Logger::SetSeverityConsoleColor(Severity severity)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (severity)
	{
	case Severity::INFO:
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case Severity::WARN:
		SetConsoleTextAttribute(hConsole, 6);
		break;
	case Severity::ERR:
		SetConsoleTextAttribute(hConsole, 12);
		break;
	}
}
