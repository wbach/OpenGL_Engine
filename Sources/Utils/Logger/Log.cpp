#include "Log.h"
#include "SDL2/SDL_messagebox.h"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>

CLogger& CLogger::Instance()
{
	static CLogger logger;
	return logger;
}

void CLogger::EnableLogs()
{
	enabled = true;
	CreateLogFile();
	loggerThread_ = std::thread(std::bind(&CLogger::ProccesLog, this));
}
void CLogger::ErrorLog(const std::string& log)
{
    MessageBox(SDL_MESSAGEBOX_ERROR, "Error", log.c_str());
    Logg(log);
}
void CLogger::Logg(const std::string& log)
{
	if (!enabled)
		return;

	std::lock_guard<std::mutex> lk(printMutex_);
	logs.push_back(log);
}
void CLogger::LoggToFileOnly(const std::string & log)
{
	std::lock_guard<std::mutex> lk(printMutex_);

	std::ofstream file(fileName, std::ios_base::app);
	file << log << '\n';
	file.close();
}
void CLogger::MessageBox(uint32 flags, const std::string& title, const std::string& message)
{
    return;
	SDL_ShowSimpleMessageBox(flags,
        title.c_str(),
        message.c_str(),
		NULL);
}
void CLogger::SaveToFile() const
{
    std::ofstream file(fileName, std::ios_base::app);
    for (const auto& log : logs)
		file << log.c_str() << '\n';
	file.close();
}
CLogger::~CLogger()
{
	if (enabled)
	{
		running_.store(false);
		loggerThread_.join();
	}
}

CLogger::CLogger()
	: running_(true)
	, timer_(5)
{	
}

void CLogger::CreateLogFile()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << in_time_t;
	system("mkdir Logs");
    fileName = "Logs/Logs.txt";
    std::ofstream file(fileName);
	file.close();
}

void CLogger::ProccesLog()
{
	while (running_.load())
	{
		std::ofstream file(fileName, std::ios_base::app);
		while (true)
		{
			auto log = GetLog();
			if (log.empty())
				break;
			std::cout << log << std::endl;
			file << log << '\n';
		}
		file.close();
		timer_.CalculateAndLock();
	}
}

std::string CLogger::GetLog()
{
	std::lock_guard<std::mutex> lk(printMutex_);

	if (logs.empty())
		return "";

	auto l = logs.front();
	logs.pop_front();
	return l;
}
