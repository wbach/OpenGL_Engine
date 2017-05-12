#pragma once
#include <string>
#include <list>
#include "../Utils/Types.h"

class CLogger
{
public:
	static CLogger& Instance();
	void EnableLogs();
	void ErrorLog(const std::string& log);
	void Logg(const std::string& log);
	void MessageBox(uint flags, const std::string& title, const std::string& message);
	void SaveToFile() const;
	~CLogger();
private:
	CLogger();
	void CreateLogFile();

	std::string m_FileName;
	std::list<std::string> m_Logs;
	bool enabled = false;
};

#define Log(x) do {CLogger::Instance().Logg(x);} while(0)
#define Error(x) do {CLogger::Instance().ErrorLog(x);} while(0)