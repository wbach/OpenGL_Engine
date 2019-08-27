#pragma once
#include <string>
#include <vector>

namespace Utils
{
struct File
{
    enum class Type
    {
        Directory,
        RegularFile,
        Other
    };

    Type type;
    std::string name;
};

void PrintFilesInDirectory(const std::string& dirPath, const std::string& t = "");
std::vector<File> GetFilesInDirectory(const std::string& dirPath);
std::string GetParent(const std::string&);
std::string GetCurrentDir();
bool DirectoryExist(const std::string&);
}  // namespace Utils
