#include "Configuration.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>
#include <fstream>

#include "ConfigurationWriter.h"
#include "Logger/Log.h"
#include "ReadConfigurationFunctions.h"

namespace GameEngine
{
namespace
{
std::vector<std::filesystem::path> requiredFiles;
}

std::filesystem::path GetDataLocationFromString(const std::string& str)
{
    if (str.empty())
        return DEFAULT_DATA_PATH;

    return str;
}

std::filesystem::path GetCacheLocationFromString(const std::string& str)
{
    if (str.empty())
        return DEFAULT_CACHE_PATH;

    return str;
}

std::filesystem::path GetShaderLocationFromString(const std::string& str)
{
    if (str.empty())
        return DEFAULT_SHADERS_PATH;

    return str;
}

void AddToRequierdFilesIfNotExist(const std::filesystem::path& file)
{
    if (std::find(requiredFiles.begin(), requiredFiles.end(), file) == requiredFiles.end())
    {
        requiredFiles.push_back(file);
    }
}

std::filesystem::path GetFullDataPath(const std::filesystem::path& fileName, bool addToRequierd)
{
    std::filesystem::path path;
    if (fileName.is_absolute())
    {
        path = fileName;
    }
    else
    {
        path = EngineConf.files.data / fileName;
    }

    if (addToRequierd)
    {
        AddToRequierdFilesIfNotExist(fileName);
    }
    return path;
}

std::filesystem::path GetFullShaderPath(const std::filesystem::path& filename, bool addToRequierd)
{
    auto path = EngineConf.files.shaders / filename;
    if (addToRequierd)
        requiredFiles.push_back(path.string());
    return path;
}

std::filesystem::path GetFilePatch(const std::filesystem::path& fileFullPath)
{
    if (fileFullPath.empty())
        return {};

    const std::filesystem::path data_root    = EngineConf.files.data;
    const std::filesystem::path shaders_root = EngineConf.files.shaders;

    if (fileFullPath.string().starts_with(data_root.string()))
        return std::filesystem::relative(fileFullPath, data_root);

    if (fileFullPath.string().starts_with(shaders_root.string()))
        return std::filesystem::relative(fileFullPath, shaders_root);

    return fileFullPath;
}

void AddRequiredFile(const std::filesystem::path& file)
{
    AddToRequierdFilesIfNotExist(file);
}

void SaveRequiredFiles()
{
    std::ofstream output(GetFullDataPath(EngineConf.files.requiredFilesOutputFile));

    if (!output.is_open())
    {
        LOG_ERROR << "Cant open file : " << GetFullDataPath(EngineConf.files.requiredFilesOutputFile);
        return;
    }

    for (const auto& str : requiredFiles)
    {
        output << str << std::endl;
    }
    output.close();
}

void CreateDefaultFile(const std::filesystem::path& filename)
{
    auto parentPath = filename.parent_path();
    if (not std::filesystem::exists(parentPath))
    {
        Utils::CreateDirectories(parentPath);
    }
    WriteConfigurationToFile(EngineConf, filename);
}

void ReadFromFile(const std::filesystem::path& filename)
{
    if (not EngineConf.filename.empty())
    {
        LOG_DEBUG << "Configuration already read. Skip";
        return;
    }
    if (Utils::CheckFileExist(filename))
    {
        ReadConfiguration(EngineConf, filename);
    }
    else
    {
        CreateDefaultFile(filename);
    }

    AddRequiredFile(filename);
    EngineConf.filename = filename.filename().string();
}
std::filesystem::path RemoveDataPath(const std::filesystem::path& path)
{
    return GetFilePatch(path);
}

std::filesystem::path GetRelativeDataPath(const std::filesystem::path& str)
{
    auto absoluteDataPath = Utils::GetAbsolutePath(EngineConf.files.data);
    return Utils::GetRelativePath(str, absoluteDataPath);
}

Configuration::Configuration()
{
}

Params::DebugParams::DebugParams()
    : logLvl{LoggingLvl::None,
             {LoggingLvl::None, LoggingLvl::Error, LoggingLvl::ErrorWarning, LoggingLvl::ErrorWarningInfo,
              LoggingLvl::ErrorWarningInfoDebug}}
    , showRenderInfo{false, {false, true}}
    , showPhycicsVisualization{false, {false, true}}
{
}

}  // namespace GameEngine
