#pragma once
#include <filesystem>
#include <optional>
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

std::string CreateUniqueFilename();
void CreateEmptyFile(const std::filesystem::path&);
bool CreateDirectories(const std::filesystem::path&);
void RenameFile(const std::string&, const std::string&);
std::filesystem::path FindFile(const std::filesystem::path&, const std::filesystem::path&);
bool IsFileExistsInDir(const std::string&, const std::string&);
std::vector<std::filesystem::path> FindFilesWithExtension(const std::filesystem::path&, const std::string&);
std::string GetFileName(const std::string&);
std::string GetExtension(const std::string&);
std::string GetFileBaseName(const std::string&);
void PrintFilesInDirectory(const std::string& dirPath, const std::string& t = "");
std::vector<File> GetFilesInDirectory(const std::string& dirPath);
std::string GetParent(const std::string&);
std::string GetCurrentDir();
std::filesystem::path GetAbsolutePath(const std::filesystem::path&);
std::string GetAbsoluteParentPath(const std::string&);
std::filesystem::path GetRelativePath(const std::filesystem::path&, const std::filesystem::path&);
bool IsAbsolutePath(const std::string&);
bool DirectoryExist(const std::filesystem::path&);
bool IsRelativePath(const std::string&);
std::string CreateBackupFile(const std::string&);
std::string ReadFileBinary(const std::filesystem::path& file_name);
std::string ReadFile(const std::filesystem::path& file_name);
std::string ReadFilesWithIncludes(const std::filesystem::path& filename);
std::vector<std::string> ReadFileLines(const std::filesystem::path& file_name);
void WrtieToFile(const std::string& filename, const std::string& content);
void CopyFileOrFolder(const std::filesystem::path& src, const std::filesystem::path& destFolder);
std::optional<std::filesystem::path> MoveFileToDirectory(const std::filesystem::path&, const std::filesystem::path&);
std::optional<std::filesystem::path> CopyFileToDirectory(const std::filesystem::path& file,
                                                         const std::filesystem::path& newParentDir);
std::filesystem::path ChangeFileParentPath(const std::filesystem::path& file, const std::filesystem::path& newParentDir);
std::optional<std::filesystem::path> GetRelativePathToFile(const std::filesystem::path& targetRelative,
                                                           const std::filesystem::path& currentPath);
}  // namespace Utils
