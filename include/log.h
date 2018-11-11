#pragma once

using namespace std;

#include <string>
#include <ctime>

// Wrappers to make logging cleaner.

//extern FILE *LogFile;
//extern bool gLogDebug;

// Note that for now I've left the definitions of LogFile and LogDebug as they
// were - either declared locally in a file, as an extern, or from another
// namespace altogether. At some point this needs to be cleaned up, but it's
// probably not worth doing so unless we were switching to use a central
// logging framework.

void addLog(const char* fmt, ...);
void addLogw(const wchar_t* fmt, ...);

#define LogInfo(fmt, ...) \
	do { addLog("[INFO]" fmt, __VA_ARGS__); } while (0)
#define vLogInfo(fmt, va_args) \
	do { addLog("[INFO]" fmt, va_args); } while (0)
#define LogInfoW(fmt, ...) \
	do { addLogw("[INFO]" fmt, __VA_ARGS__); } while (0)
#define vLogInfoW(fmt, va_args) \
	do { addLogw("[INFO]" fmt, va_args); } while (0)

#define LogDebug(fmt, ...) \
	do { addLog("[DEBUG]" fmt, __VA_ARGS__); } while (0)
#define vLogDebug(fmt, va_args) \
	do { addLog("[DEBUG]" fmt, va_args); } while (0)
#define LogDebugW(fmt, ...) \
	do { addLogw("[DEBUG]" fmt, __VA_ARGS__); } while (0)

// Aliases for the above functions that we use to denote that omitting the
// newline was done intentionally. For now this is just for our reference, but
// later we might actually make the default function insert a newline:
#define LogInfoNoNL LogInfo
#define LogInfoWNoNL LogInfoW
#define LogDebugNoNL LogDebug
#define LogDebugWNoNL LogDebugW

static string LogTime()
{
	string timeStr;
	char cTime[32];
	tm timestruct;

	time_t ltime = time(0);
	localtime_s(&timestruct, &ltime);
	asctime_s(cTime, sizeof(cTime), &timestruct);

	timeStr = cTime;
	return timeStr;
}

