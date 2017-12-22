#pragma once
#include "../Types.h"
#include <string>
#include <list>

class CLogger
{
public:
	static CLogger& Instance();
	void EnableLogs();
	void ErrorLog(const std::string& log);
	void Logg(const std::string& log);
	void MessageBox(uint32 flags, const std::string& title, const std::string& message);
	void SaveToFile() const;
	~CLogger();
private:
	CLogger();
	void CreateLogFile();

private:
    std::string fileName;
    std::list<std::string> logs;
	bool enabled = false;
};

#define Log(x) do {CLogger::Instance().Logg(x);} while(0)
#define Error(x) do {CLogger::Instance().ErrorLog(x);} while(0)
