#include "Log.h"
#include "SDL2/SDL.h"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>

CLogger & CLogger::Instance()
{
	static CLogger logger;
	return logger;
}

void CLogger::EnableLogs()
{
	enabled = true;
	CreateLogFile();
}
void CLogger::ErrorLog(const std::string& log)
{
	MessageBox(SDL_MESSAGEBOX_ERROR, "Error", log.c_str());
	Log("[Error] " + log);
}
void CLogger::Logg(const std::string& log)
{
	if (!enabled)
		return;
	std::cout << log << std::endl;
	m_Logs.push_back(log);
	std::ofstream file(m_FileName, std::ios_base::app);
	file << log << '\n';
	file.close();
}
void CLogger::MessageBox(uint flags, const std::string& title, const std::string& message)
{
	SDL_ShowSimpleMessageBox(flags,
		title.c_str(),
		message.c_str(),
		NULL);
}
void CLogger::SaveToFile() const
{
	std::ofstream file(m_FileName, std::ios_base::app);
	for (const auto& log : m_Logs)
		file << log.c_str() << '\n';
	file.close();
}
CLogger::~CLogger()
{
}

CLogger::CLogger()
{

}

void CLogger::CreateLogFile()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << in_time_t;
	m_FileName = "Logs/Logs" + ss.str() + ".txt";
	std::ofstream file(m_FileName);
	file.close();
}
