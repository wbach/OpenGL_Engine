#include "FileSystemUtils.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
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

    directory_iterator end_itr;

    for (directory_iterator itr(p); itr != end_itr; ++itr)
    {
        if (is_regular_file(itr->path()))
        {
            result.push_back({File::Type::RegularFile, itr->path().string()});
        }
        else if (is_directory(itr->path()))
        {
            result.push_back({File::Type::Directory, itr->path().string()});
        }
        else
        {
            result.push_back({File::Type::Other, itr->path().string()});
        }
    }

    return result;
}
}  // namespace Utils