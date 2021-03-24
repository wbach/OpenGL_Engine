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
void CreateEmptyFile(const std::string&);
void RenameFile(const std::string&, const std::string&);
std::string FindFile(const std::string&, const std::string&);
std::vector<std::string> FindFilesWithExtension(const std::string&, const std::string&);
std::string GetFileName(const std::string&);
std::string GetExtension(const std::string&);
std::string GetFileBaseName(const std::string&);
void PrintFilesInDirectory(const std::string& dirPath, const std::string& t = "");
std::vector<File> GetFilesInDirectory(const std::string& dirPath);
std::string GetParent(const std::string&);
std::string GetCurrentDir();
std::string GetAbsolutePath(const std::string&);
std::string GetAbsoluteParentPath(const std::string&);
std::string GetRelativePath(const std::string&, const std::string&);
bool IsAbsolutePath(const std::string&);
bool IsRelativePath(const std::string&);
bool DirectoryExist(const std::string&);
std::string CreateBackupFile(const std::string&);
std::string ReadFileBinary(const std::string& file_name);
std::string ReadFile(const std::string& file_name);
std::string ReadFilesWithIncludes(const std::string& filename);
std::vector<std::string> ReadFileLines(const std::string& file_name);
void WrtieToFile(const std::string& filename, const std::string& content);
}  // namespace Utils
