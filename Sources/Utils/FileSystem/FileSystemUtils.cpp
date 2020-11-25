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
std::string FindFile(const std::string& filename, const std::string& dir)
{
    try
    {
        for (auto& p : std::filesystem::directory_iterator(GetAbsolutePath(dir)))
        {
            if (p.is_directory())
            {
                auto maybeFileName = FindFile(filename, p.path().string());
                if (not maybeFileName.empty())
                    return maybeFileName;
            }
            else
            {
                if (p.path().filename() == filename)
                    return Utils::ReplaceSlash(p.path().string());
            }
        }
    }
    catch (...)
    {
        ERROR_LOG("Find file error.");
    }

    return std::string();
}
std::string GetFileName(const std::string& filename)
{
    return std::filesystem::path(filename).filename().string();
}
std::string GetExtension(const std::string& filename)
{
    return std::filesystem::path(filename).extension().string();
}
std::string GetFileBaseName(const std::string& filename)
{
    return std::filesystem::path(filename).stem().string();
}
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
        auto lname = std::to_string(static_cast<int>(l.type)) + Utils::GetFileName(l.name);
        auto rname = std::to_string(static_cast<int>(r.type)) + Utils::GetFileName(r.name);
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
    try
    {
        return std::filesystem::canonical(file).string();
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        ERROR_LOG(e.what());
        return file;
    }
    catch (...)
    {
        ERROR_LOG("error " + file);
        return file;
    }
}

std::string GetAbsoluteParentPath(const std::string& file)
{
    try
    {
        return std::filesystem::canonical(file).parent_path().string();
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        ERROR_LOG(e.what());
        return file;
    }
    catch (...)
    {
        ERROR_LOG("error " + file);
        return file;
    }
}

bool DirectoryExist(const std::string& pathDir)
{
    return std::filesystem::exists(pathDir) and std::filesystem::is_directory(pathDir);
}

bool IsAbsolutePath(const std::string& path)
{
    return std::filesystem::path(path).is_absolute();
}

bool IsRelativePath(const std::string& path)
{
    return std::filesystem::path(path).is_relative();
}

std::string GetRelativePath(const std::string& absoultePath, const std::string& workingPath)
{
    try
    {
        return std::filesystem::relative(absoultePath, workingPath).string();
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        ERROR_LOG(e.what());
        return absoultePath;
    }
    catch (...)
    {
        ERROR_LOG("error " + absoultePath);
        return absoultePath;
    }
}

std::string CreateBackupFile(const std::string& output)
{
    if (std::filesystem::exists(output))
    {
        try
        {
            auto backupFile = output + ".backup";
            if (std::filesystem::exists(backupFile))
            {
                DEBUG_LOG("Remove old backup file " + backupFile);
                std::filesystem::remove(backupFile);
            }

            std::filesystem::copy(output, backupFile);
            DEBUG_LOG("Backup created. " + backupFile);
            return backupFile;
        }
        catch (...)
        {
            ERROR_LOG("Create backup error. " + output);
        }
    }

    return {};
}

void ReadFilesWithIncludesImpl(const std::string& fullPath, std::stringstream& output)
{
    const std::string includeStr{"#include"};

    auto lines = Utils::ReadFileLines(fullPath);

    for (const auto& line : lines)
    {
        auto lineWithInclude = line.find(includeStr);

        if (lineWithInclude != std::string::npos)
        {
            auto startFileNamePos = line.find_first_of('"') + 1;
            auto endNamePos       = line.find_last_of('"');

            if (startFileNamePos >= endNamePos)
            {
                ERROR_LOG(fullPath + " : inncorect include line : " + line);
                continue;
            }

            auto filename = line.substr(startFileNamePos, endNamePos - startFileNamePos);
            std::string includedFileName{std::filesystem::path(filename).make_preferred().string()};
            auto absultePath =
                std::filesystem::canonical(std::filesystem::canonical(fullPath).replace_filename(includedFileName))
                    .string();
            ReadFilesWithIncludesImpl(absultePath, output);
            output << '\n';
        }
        else
        {
            output << line << '\n';
        }
    }
}

std::string ReadFilesWithIncludes(const std::string& filename)
{
    std::stringstream sourceCode;
    try
    {
        ReadFilesWithIncludesImpl(filename, sourceCode);
    }
    catch (...)
    {
        ERROR_LOG(filename + " including error");
    }
    return sourceCode.str();
}

}  // namespace Utils
