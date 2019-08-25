#include "FileSystemUtils.hpp"
#include <Logger/Log.h>
#include <Utils/Utils.h>
#include <boost/filesystem.hpp>
#include <iostream>

#ifndef USE_GNU
#include <Windows.h>
#endif

using namespace boost::filesystem;

namespace Utils
{
void PrintFilesInDirectory(const std::string& dirPath, const std::string& t)
{
    path p(dirPath);

    directory_iterator end_itr;

    for (directory_iterator itr(p); itr != end_itr; ++itr)
    {
        if (is_regular_file(itr->path()))
        {
            std::string current_file = itr->path().string();
            std::cout << t << "[F] " << current_file << std::endl;
        }
        else if (is_directory(itr->path()))
        {
            std::string current_file = itr->path().string();
            std::cout << t << "[D] " << current_file << std::endl;
            PrintFilesInDirectory(current_file, t + "    ");
        }
    }
}
std::vector<File> GetFilesInDirectory(const std::string& dirPath)
{
    std::vector<File> result;
    path p(dirPath);

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

    directory_iterator end_itr;

    for (directory_iterator itr(p); itr != end_itr; ++itr)
    {
        auto filename = Utils::ReplaceSlash(itr->path().string());

        if (is_regular_file(itr->path()))
        {
            result.push_back({File::Type::RegularFile, filename});
        }
        else if (is_directory(itr->path()))
        {
            result.push_back({File::Type::Directory, filename});
        }
        else
        {
            result.push_back({File::Type::Other, filename});
        }
    }

    return result;
}
std::string GetParent(const std::string& dir)
{
    auto result = path(dir).parent_path().string();
    auto slash = result.find_last_of('/');

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
    return Utils::ReplaceSlash(boost::filesystem::path(boost::filesystem::current_path()).string());
}
}  // namespace Utils
