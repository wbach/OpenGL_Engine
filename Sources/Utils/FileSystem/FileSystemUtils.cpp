#include "FileSystemUtils.hpp"

#include <Logger/Log.h>
#include <Utils/Utils.h>

#include <algorithm>
#include <iostream>
#include <optional>
#include <random>

#ifndef USE_GNU
#include <windows.h>
#endif

#include <filesystem>
#include <fstream>

namespace Utils
{
namespace fs = std::filesystem;

std::filesystem::path FindFile(const std::filesystem::path& filepath, const std::filesystem::path& dir)
{
    try
    {
        if (std::filesystem::exists(dir))
        {
            for (auto& p : std::filesystem::directory_iterator(std::filesystem::canonical(dir)))
            {
                if (p.is_directory())
                {
                    if (p.path().string() != dir)
                    {
                        auto maybeFileName = FindFile(filepath, p.path());
                        if (not maybeFileName.empty())
                            return maybeFileName;
                    }
                }
                else
                {
                    if (p.path().filename() == filepath)
                        return p.path();
                }
            }
        }
    }
    catch (...)
    {
        LOG_ERROR << "Find file error. searching file : " << filepath << " in dir : " << dir;
    }

    return {};
}

bool IsFileExistsInDir(const std::string& directory, const std::string& filename)
{
    if (not fs::exists(directory) or not fs::is_directory(directory))
    {
        return false;
    }

    for (const auto& entry : fs::recursive_directory_iterator(directory))
    {
        if (entry.is_regular_file() and entry.path().filename() == filename)
        {
            return true;
        }
    }

    return false;
}

std::vector<std::filesystem::path> FindFilesWithExtension(const std::filesystem::path& dir, const std::string& extension)
{
    std::vector<std::filesystem::path> result;

    try
    {
        if (not dir.empty())
        {
            for (auto& p : std::filesystem::directory_iterator(std::filesystem::canonical(dir)))
            {
                if (p.is_directory())
                {
                    auto subResult = FindFilesWithExtension(p.path().string(), extension);
                    result.insert(result.end(), subResult.begin(), subResult.end());
                }
                else
                {
                    if (p.path().extension() == extension)
                        result.push_back(p.path().string());
                }
            }
        }
    }
    catch (...)
    {
        LOG_ERROR << "Find files error. searching files with extension : " << extension << " in dir : " << dir;
    }

    return result;
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
            LOG_DEBUG << t << "[D] " << filename;

            PrintFilesInDirectory(p.path().string(), t + "    ");
        }
        else
        {
            auto filename = Utils::ReplaceSlash(p.path().string());
            LOG_DEBUG << t << "[F] " << filename;
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
        GetLogicalDriveStringsA(mydrives, tmp);
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

    std::sort(result.begin(), result.end(),
              [](const File& l, const File& r)
              {
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

std::filesystem::path GetAbsolutePath(const std::filesystem::path& file)
{
    try
    {
        return std::filesystem::canonical(file).string();
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        LOG_ERROR << e.what();
        return file;
    }
    catch (...)
    {
        LOG_ERROR << "error " << file;
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
        /* LOG TO FIX*/ LOG_ERROR << (e.what());
        return file;
    }
    catch (...)
    {
        /* LOG TO FIX*/ LOG_ERROR << ("error " + file);
        return file;
    }
}

bool DirectoryExist(const std::filesystem::path& pathDir)
{
    try
    {
        return std::filesystem::exists(pathDir) and std::filesystem::is_directory(pathDir);
    }
    catch (...)
    {
        return false;
    }
}

bool IsAbsolutePath(const std::string& path)
{
    try
    {
        return std::filesystem::path(path).is_absolute();
    }
    catch (...)
    {
        return false;
    }
}

bool IsRelativePath(const std::string& path)
{
    try
    {
        return std::filesystem::path(path).is_relative();
    }
    catch (...)
    {
        return false;
    }
}

std::filesystem::path GetRelativePath(const std::filesystem::path& absoultePath, const std::filesystem::path& workingPath)
{
    try
    {
        return std::filesystem::relative(absoultePath, workingPath).string();
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        LOG_ERROR << e.what();
        return absoultePath;
    }
    catch (...)
    {
        LOG_ERROR << "error " << absoultePath;
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
                /* LOG TO FIX*/ LOG_ERROR << ("Remove old backup file " + backupFile);
                std::filesystem::remove(backupFile);
            }

            std::filesystem::copy(output, backupFile);
            /* LOG TO FIX*/ LOG_ERROR << ("Backup created. " + backupFile);
            return backupFile;
        }
        catch (...)
        {
            /* LOG TO FIX*/ LOG_ERROR << ("Create backup error. " + output);
        }
    }

    return {};
}

void ReadFilesWithIncludesImpl(const std::filesystem::path& fullPath, std::stringstream& output)
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
                LOG_ERROR << fullPath << " : inncorect include line : " << line;
                continue;
            }

            auto filename = line.substr(startFileNamePos, endNamePos - startFileNamePos);
            try
            {
                std::string includedFileName{std::filesystem::path(filename).make_preferred().string()};
                auto absultePath =
                    std::filesystem::canonical(std::filesystem::canonical(fullPath).replace_filename(includedFileName)).string();
                ReadFilesWithIncludesImpl(absultePath, output);
                output << '\n';
            }
            catch (...)
            {
                output << line << '\n';
            }
        }
        else
        {
            output << line << '\n';
        }
    }
}

std::string ReadFilesWithIncludes(const std::filesystem::path& filename)
{
    std::stringstream sourceCode;
    try
    {
        ReadFilesWithIncludesImpl(filename, sourceCode);
    }
    catch (...)
    {
        LOG_ERROR << filename << " including error";
    }
    return sourceCode.str();
}
std::string CreateUniqueFilename()
{
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms   = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y%m%d_%H%M%S") << '_' << std::setfill('0') << std::setw(3) << ms.count();

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1000, 9999);
    oss << '_' << dist(rng);

    return oss.str();
}
void CreateEmptyFile(const std::filesystem::path& filename)
{
    try
    {
        std::ofstream newFile;
        newFile.open(filename);
        newFile.close();
    }
    catch (...)
    {
        LOG_ERROR << "Create file error : " << filename;
    }
}
std::string ReadFileBinary(const std::filesystem::path& file_name)
{
    std::ifstream t(file_name, std::ios::binary);
    if (!t.is_open())
    {
        LOG_ERROR << "Cannot open file : " << file_name;
        return std::string();
    }
    std::string out = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();
    return out;
}

std::string ReadFile(const std::filesystem::path& file_name)
{
    std::ifstream t(file_name);
    if (!t.is_open())
    {
        LOG_ERROR << "Cannot open file : " << file_name;
        return std::string();
    }
    std::string out = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();
    return out;
}

std::vector<std::string> ReadFileLines(const std::filesystem::path& file_name)
{
    std::vector<std::string> output;
    std::ifstream f(file_name);
    if (!f.is_open())
    {
        LOG_ERROR << "Cannot open file : " << file_name;
        return output;
    }

    std::string line;

    while (std::getline(f, line))
        output.push_back(line);

    f.close();
    return output;
}
void RenameFile(const std::string& path, const std::string& newName)
{
    try
    {
        std::filesystem::rename(path, newName);
    }
    catch (...)
    {
        /* LOG TO FIX*/ LOG_ERROR << ("rename error");
    }
}
void CopyFileOrFolder(const std::filesystem::path& src, const std::filesystem::path& destFolder)
{
    std::filesystem::path dstPath = destFolder / src.filename();

    // Konflikt nazwy
    if (std::filesystem::exists(dstPath))
    {
        int counter                   = 1;
        std::filesystem::path baseDst = dstPath;
        do
        {
            dstPath = baseDst.stem().string() + "_" + std::to_string(counter) + dstPath.extension().string();
            dstPath = destFolder / dstPath.filename();
            counter++;
        } while (std::filesystem::exists(dstPath));
    }

    try
    {
        if (std::filesystem::is_directory(src))
        {
            std::function<void(const std::filesystem::path&, const std::filesystem::path&)> copyRecursive;
            copyRecursive = [&](const std::filesystem::path& srcDir, const std::filesystem::path& dstDir)
            {
                Utils::CreateDirectories(dstDir);
                for (auto& entry : std::filesystem::directory_iterator(srcDir))
                {
                    auto dstEntry = dstDir / entry.path().filename();
                    if (entry.is_directory())
                        copyRecursive(entry.path(), dstEntry);
                    else if (entry.is_regular_file())
                        std::filesystem::copy_file(entry.path(), dstEntry);
                }
            };
            copyRecursive(src, dstPath);
        }
        else if (std::filesystem::is_regular_file(src))
        {
            std::filesystem::copy_file(src, dstPath);
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        LOG_ERROR << "Copy failed: " << e.what();
    }
}

std::optional<std::filesystem::path> CopyFileToDirectory(const std::filesystem::path& file,
                                                         const std::filesystem::path& newParentDir)
{
    try
    {
        if (not std::filesystem::exists(file) or not std::filesystem::is_regular_file(file))
        {
            std::cerr << "File not found: " << file << "\n";
            return std::nullopt;
        }

        if (not std::filesystem::exists(newParentDir) or not std::filesystem::is_directory(newParentDir))
        {
            std::cerr << "Parent dir not found: " << newParentDir << "\n";
            return std::nullopt;
        }

        std::filesystem::path newPath = newParentDir / file.filename();
        std::filesystem::copy_file(file, newPath, std::filesystem::copy_options::overwrite_existing);

        return newPath;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return std::nullopt;
    }
}

std::optional<std::filesystem::path> MoveFileToDirectory(const std::filesystem::path& file,
                                                         const std::filesystem::path& newParentDir)
{
    try
    {
        if (not std::filesystem::exists(file) or not std::filesystem::is_regular_file(file))
        {
            std::cerr << "File not found: " << file << "\n";
            return std::nullopt;
        }

        if (not std::filesystem::exists(newParentDir) or not std::filesystem::is_directory(newParentDir))
        {
            std::cerr << "Parent dir not found: " << newParentDir << "\n";
            return std::nullopt;
        }

        std::filesystem::path newPath = newParentDir / file.filename();

        try
        {
            std::filesystem::rename(file, newPath);
        }
        catch (const std::filesystem::filesystem_error&)
        {
            std::filesystem::copy_file(file, newPath, std::filesystem::copy_options::overwrite_existing);
            std::filesystem::remove(file);
        }

        return newPath;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return std::nullopt;
    }
}

std::filesystem::path ChangeFileParentPath(const std::filesystem::path& file, const std::filesystem::path& newParentDir)
{
    if (not file.empty() and not newParentDir.empty())
    {
        return newParentDir / file.filename();
    }
    else
    {
        return file;
    }
}

std::optional<std::filesystem::path> GetRelativePathToFile(const std::filesystem::path& baseDir,
                                                           const std::filesystem::path& filePath)
{
    auto absBase = baseDir.lexically_normal();
    auto absFile = filePath.lexically_normal();

    auto fileParent = absFile.parent_path();

    auto relative = fileParent.lexically_relative(absBase);

    // przypadek: nie da się policzyć lub wyszło poza baseDir
    if (relative.empty() && fileParent != absBase)
    {
        return std::nullopt;
    }
    if (!relative.empty() && relative.begin()->string() == "..")
    {
        return std::nullopt;
    }

    // przypadek: ta sama ścieżka => chcemy ""
    if (relative == ".")
    {
        return std::filesystem::path{};
    }

    return relative;
}
bool CreateDirectories(const std::filesystem::path& path)
{
    try
    {
        if (std::filesystem::exists(path))
        {
            LOG_DEBUG << "Path already exists: " << path;
            return true;
        }

        if (std::filesystem::create_directories(path))
        {
            LOG_DEBUG << "Directories created: " << path;
            return true;
        }
        else
        {
            LOG_ERROR << "Failed to create directories: " << path;
            return false;
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << "Error creating directories (" << path << "): " << e.what();
        return false;
    }
}
}  // namespace Utils
