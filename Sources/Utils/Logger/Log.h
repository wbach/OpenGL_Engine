#pragma once
#include "../Types.h"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "Time/TimeMeasurer.h"
#include <string>
#include <list>
#include <atomic>

class CLogger
{
public:
	static CLogger& Instance();
	void EnableLogs();
	void ErrorLog(const std::string& log);
	void Logg(const std::string& log);
	void LoggToFileOnly(const std::string& log);
	void MessageBox(uint32 flags, const std::string& title, const std::string& message);
	void SaveToFile() const;
	~CLogger();
private:
	CLogger();
	void CreateLogFile();
	void ProccesLog();
	std::string GetLog();

private:
	std::mutex printMutex_;
    std::string fileName;
    std::list<std::string> logs;
	bool enabled = false;

	std::atomic_bool running_;
	std::thread loggerThread_;
	Utils::Time::CTimeMeasurer timer_;
};

#define LogToFile(x)// do {CLogger::Instance().LoggToFileOnly(x);} while(0)
#define Log(x)// do {CLogger::Instance().Logg(__FUNCTION__ + std::string(": ") + x);} while(0)
#define Error(x) do {CLogger::Instance().ErrorLog(x);} while(0)
