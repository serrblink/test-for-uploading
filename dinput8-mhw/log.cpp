#include "log.h"

#include <fstream>

static char* getLogFilename() {
	static char tmp[256] = "intercept.log";
	GetEnvironmentVariableA("LOGFILE", tmp, sizeof(tmp));
	return tmp;
}
static char* log_filename = getLogFilename();


LogLevel min_log_level = INFO;

__declspec(dllexport) void _log(int l, const char* s) {

	if (l >= min_log_level) {
		static std::ofstream o(log_filename);
		time_t mytime = time(NULL);
		tm mytm;
		localtime_s(&mytm, &mytime);
		char stamp[128] = { 0 };
		strftime(stamp, sizeof(stamp), "%H:%M:%S", &mytm);
		o << "[ " << stamp << " ] " << s;
		o.flush();
		{
			static HANDLE console = 0;
			if (!console) {
				AllocConsole();
				SetConsoleTitle("Intercept Log");
				console = GetStdHandle(STD_OUTPUT_HANDLE);
			}
			SetConsoleTextAttribute(console, FOREGROUND_GREEN);
			WriteConsole(console, "[ ", 2, nullptr, 0);
			WriteConsole(console, stamp, (DWORD)strlen(stamp), nullptr, 0);
			WriteConsole(console, " ] ", 3, nullptr, 0);

			if (l == INFO || l == DEBUG)
				SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
			if (l == WARN)
				SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
			if (l == ERR)
				SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
			WriteConsole(console, s, (DWORD)strlen(s), nullptr, 0);
			SetConsoleTextAttribute(console, 0);
		}
	}
}

LOG::~LOG() {
	s_ << std::endl;
	s_.flush();
	_log((int)l_, s_.str().c_str());
}
