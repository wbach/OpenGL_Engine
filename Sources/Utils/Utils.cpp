#include "Utils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>

#include "FileSystem/FileSystemUtils.hpp"
#include "Logger/Log.h"


namespace Utils
{
std::vector<std::string> SplitString(char* s, int size, char split_char)
{
    std::vector<std::string> tokens;
    std::string token;

    for (int i = 0; i < size; ++i)
    {
        if (s[i] == split_char)
        {
            if (not token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }
        token += s[i];
    }
    if (not token.empty())
        tokens.push_back(token);

    return tokens;
}
std::vector<std::string> SplitString(const std::string& s, char split_char)
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

std::string ReadFileBinary(const std::string& file_name)
{
    std::ifstream t(file_name, std::ios::binary);
    if (!t.is_open())
    {
        ERROR_LOG("Cannot open file : " + file_name);
        return std::string();
    }
    std::string out = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();
    return out;
}

std::string ReadFile(const std::string& file_name)
{
    std::ifstream t(file_name);
    if (!t.is_open())
    {
        ERROR_LOG("Cannot open file : " + file_name);
        return std::string();
    }
    std::string out = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();
    return out;
}

std::vector<std::string> ReadFileLines(const std::string& file_name)
{
    std::vector<std::string> output;
    std::ifstream f(file_name);
    if (!f.is_open())
    {
        ERROR_LOG("Cannot open file : " + file_name);
        return output;
    }

    std::string line;

    while (std::getline(f, line))
        output.push_back(line);

    f.close();
    return output;
}

void WrtieToFile(const std::string& filename, const std::string& content)
{
    std::ofstream file(filename.c_str());
    if (!file.is_open())
    {
        ERROR_LOG("cannot open file " + filename);
        return;
    }
    file << content;
    file.close();
}

std::string ReplaceSlash(const std::string& str)
{
    std::string result(str);
    std::replace(result.begin(), result.end(), '\\', '/');
    return result;
}

bool IsWindowsDriveName(const std::string& path)
{
    return (path.size() == 3 and path[1] == ':' and path[2] == '/');
}

std::string GetFilename(const std::string& fullpath)
{
    auto file = ReplaceSlash(fullpath);

    if (IsWindowsDriveName(file))
    {
        file.pop_back();
        return file;
    }

    auto filename = file.substr(file.find_last_of('/') + 1);

    auto p = filename.find_last_of('.');
    if (p != std::string::npos)
    {
        filename = filename.substr(0, p);
    }
    return filename;
}

std::string GetFilePath(const std::string& fullpath)
{
    auto file = ReplaceSlash(fullpath);
    return file.substr(0, file.find_last_of('/'));
}

std::string GetFileExtension(const std::string& file_name)
{
    if (file_name.empty())
        return {};

    auto pos = file_name.find_last_of('.') + 1;

    if (pos >= file_name.size())
        return {};

    return file_name.substr(pos);
}

std::string GetFilenameWithExtension(const std::string& fullpath)
{
    auto file = ReplaceSlash(fullpath);

    if (IsWindowsDriveName(file))
    {
        file.pop_back();
        return file;
    }
    return file.substr(file.find_last_of('/') + 1);
}

std::string GetPathAndFilenameWithoutExtension(const std::string& path)
{
    auto file = ReplaceSlash(path);
    if (IsWindowsDriveName(file))
    {
        file.pop_back();
        return file;
    }

    return file.substr(0, file.find_last_of('.'));
}

std::string ConvertToRelativePath(std::string path)
{
    if (path.empty())
        return path;

    std::replace(path.begin(), path.end(), '\\', '/');
    std::size_t found = path.find("Data/");
    if (found != std::string::npos)
    {
        path = path.substr(found);
    }
    return path;
}

int NextP2(int a)
{
    int rval = 1;
    while (rval < a)
        rval <<= 1;
    return rval;
}
bool CheckFileExist(const std::string& file)
{
    std::ifstream f(file);
    bool exist = f.is_open();
    f.close();
    return exist;
}

bool CheckExtension(const std::string& file, const std::string& ext)
{
    auto pos = file.find_last_of('.');
    if (pos == std::string::npos)
        return false;

    auto extInFile = file.substr(pos + 1);
    return extInFile == ext;
}

float StringToFloat(const std::string& str)
{
    float f = 0.0f;
    try
    {
        f = std::stof(str);
    }
    catch (const std::invalid_argument& e)
    {
        std::string s = e.what();
        ERROR_LOG("StringToFloat invalid_argument. : " + s);
    }
    catch (const std::out_of_range& e)
    {
        std::string s = e.what();
        ERROR_LOG("StringToFloat out_of_range. : " + s);
    }
    return f;
}

int StringToInt(const std::string& str)
{
    int i = 0;
    try
    {
        i = std::stoi(str);
    }
    catch (const std::invalid_argument& e)
    {
        std::string s = e.what();
        ERROR_LOG("StringToInt invalid_argument. : " + s);
    }
    catch (const std::out_of_range& e)
    {
        std::string s = e.what();
        ERROR_LOG("StringToInt out_of_range. : " + s);
    }
    return i;
}

bool StringToBool(const std::string& str)
{
    if (str == "1")
        return true;
    if (str == "0")
        return false;

    auto s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return (s == "true" ? true : false);
}

std::string toLower(const std::string& input)
{
    std::string result{input};
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string BoolToString(bool b)
{
    return b ? "true" : "false";
}

std::string MergeString(const std::vector<std::string>& input, const std::string& separator)
{
    std::string result;
    for (const auto& s : input)
    {
        result += s + separator;
    }
    return result;
}

}  // namespace Utils
