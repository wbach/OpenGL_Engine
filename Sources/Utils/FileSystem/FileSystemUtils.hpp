#pragma once
#include <string>
#include <vector>

namespace Utils
{
struct File
{
    enum class Type
    {
        RegularFile,
        Directory,
        Other
    };

    Type type;
    std::string name;
};

void PrintFilesInDirectory(const std::string& dirPath, const std::string& t = "");
std::vector<File> GetFilesInDirectory(const std::string& dirPath);
}  // namespace Utils