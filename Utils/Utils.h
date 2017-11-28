#pragma once
#include <glm/glm.hpp>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include "Types.h"
#include "math.hpp"

namespace Utils
{
template <typename T>
std::string ToStr(const T& t)
{
    std::ostringstream os;
    os << t;
    return os.str();
}
std::vector<std::string> SplitString(const std::string& s, char split_char);
std::string ReadFileBinary(const std::string& file_name);
std::string ReadFile(const std::string& file_name);
std::list<std::string> ReadFileLines(const std::string& file_name);
void GetFileAndPath(const std::string& full, std::string& filename, std::string& path);
std::string GetFileExtension(const std::string& file_name);
std::string ConvertToRelativePath(std::string path);
int NextP2(int a);
bool CheckFileExist(const std::string& file);
bool CheckExtension(const std::string& file, const std::string& ext);
// catch exception and log
float StringToFloat(const std::string& str);
int StringToInt(const std::string& str);
bool StringToBool(const std::string& str);
}
