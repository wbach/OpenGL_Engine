#include "FileSystemUtils.hpp"

#include <Logger/Log.h>
#include <Utils/Utils.h>

#include <algorithm>
#include <iostream>

#ifndef USE_GNU
#include <Windows.h>
#endif

#include <filesystem>

namespace Utils
{
void PrintFilesInDirectory(const std::string& dirPath, const std::string& t)
{
    for (auto& p : std::filesystem::directory_iterator(dirPath))
    {
        if (p.is_directory())
        {
            auto filename = Utils::ReplaceSlash(p.path().string());
            std::cout << t << "[D] " << filename << std::endl;

            PrintFilesInDirectory(p.path().string(), t + "    ");
        }
        else
        {
            auto filename = Utils::ReplaceSlash(p.path().string());
            std::cout << t << "[F] " << filename << std::endl;
        }
    }
}
std::vector<File> GetFilesInDirectory(const std::string& dirPath)
{
    std::vector<File> result;

#ifndef USE_GNU
    if (dirPath.empty() or dirPath == "/")
    {
        DWORD mydrives = 100;
        char tmp[100];
        ZeroMemory(tmp, 100);
        GetLogicalDriveStrings(mydrives, tmp);
        auto drives = Utils::SplitString(tmp, 90, 0);
        for (const auto& c : drives)
        {
            result.push_back({File::Type::Directory, Utils::ReplaceSlash(c)});
        }
        return result;
    }
#endif

    for (auto& p : std::filesystem::directory_iterator(dirPath))
    {
        auto filename = Utils::ReplaceSlash(p.path().string());

        if (p.is_regular_file())
        {
            result.push_back({File::Type::RegularFile, filename});
        }
        else if (p.is_directory())
        {
            result.push_back({File::Type::Directory, filename});
        }
        else
        {
            result.push_back({File::Type::Other, filename});
        }
    }

    std::sort(result.begin(), result.end(), [](const File& l, const File& r) {
        auto lname = std::to_string(static_cast<int>(l.type)) + Utils::GetFilenameWithExtension(l.name);
        auto rname = std::to_string(static_cast<int>(r.type)) + Utils::GetFilenameWithExtension(r.name);
        std::transform(lname.begin(), lname.end(), lname.begin(), ::tolower);
        std::transform(rname.begin(), rname.end(), rname.begin(), ::tolower);
        return lname < rname;
    });

    return result;
}
std::string GetParent(const std::string& dir)
{
    auto result = std::filesystem::path(dir).parent_path().string();
    auto slash  = result.find_last_of('/');

    if (slash != std::string::npos)
    {
        return result;
    }
    else
    {
        return dir;
    }
}
std::string GetCurrentDir()
{
    return Utils::ReplaceSlash(std::filesystem::path(std::filesystem::current_path()).string());
}

std::string GetAbsolutePath(const std::string& file)
{
    return std::filesystem::canonical(file).string();
}

bool DirectoryExist(const std::string& pathDir)
{
    return std::filesystem::exists(pathDir) and std::filesystem::is_directory(pathDir);
}
}  // namespace Utils
