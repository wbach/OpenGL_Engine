#include "Utils.h"
#include "../Debug_/Log.h"
#include <algorithm>
#include <fstream>

std::vector<std::string> Utils::SplitString(const std::string & s, char split_char)
{
	std::vector<std::string> tokens;
	std::string token;

	for (const auto& c : s)
	{
		if (c == split_char)
		{
			tokens.push_back(token);
			token.clear();
			continue;
		}
		token += c;
	}
	if (!token.empty())
		tokens.push_back(token);

	return tokens;
}

std::string Utils::ReadFile(const std::string & file_name)
{
	std::ifstream t(file_name);
	if (!t.is_open())
	{
		Error("Cannot open file : " + file_name);
		return std::string();
	}
	std::string out = std::string((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	t.close();
	return out;
}

void Utils::GetFileAndPath(const std::string & full, std::string & filename, std::string & path)
{
	auto file = full;
	std::replace(file.begin(), file.end(), '\\', '/');
	path = file.substr(0, file.find_last_of('/'));
	filename = file.substr(file.find_last_of('/') + 1);
}

std::string Utils::ConvertToRelativePath(std::string path)
{
	if (path.empty()) return path;

	std::replace(path.begin(), path.end(), '\\', '/');
	std::size_t found = path.find("Data/");
	if (found != std::string::npos)
	{
		path = path.substr(found);
	}
	return path;
}

int Utils::NextP2(int a)
{
	int rval = 1;
	while (rval < a) rval <<= 1;
	return rval;
}
bool Utils::CheckFileExist(const std::string & file)
{
	std::ofstream f(file);
	bool exist = f.is_open();
	f.close();
	return exist;
}

float Utils::StringToFloat(const std::string & str)
{
	float f = 0.0f;
	try
	{
		f = std::stof(str);
	}
	catch (const std::invalid_argument& e)
	{
		std::string s = e.what();
		Log("Utils::StringToFloat invalid_argument. : " + s);
	}
	catch (const std::out_of_range& e)
	{
		std::string s = e.what();
		Log("Utils::StringToFloat out_of_range. : " + s);
	}
	return f;
}

int Utils::StringToInt(const std::string & str)
{
	int i = 0;
	try
	{
		i = std::stoi(str);
	}
	catch (const std::invalid_argument& e)
	{
		std::string s = e.what();
		Log("Utils::StringToInt invalid_argument. : " + s);
	}
	catch (const std::out_of_range& e)
	{
		std::string s = e.what();
		Log("Utils::StringToInt out_of_range. : " + s);
	}
	return i;
}

bool Utils::StringToBool(const std::string & str)
{
	if (str == "1")
		return true;
	if (str == "0")
		return false;

	auto s = str;
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return (s == "true" ? true : false);
}
