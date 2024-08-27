#include "Configuration.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>
#include <fstream>

#include "ConfigurationReader.h"
#include "ConfigurationWriter.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace
{
std::vector<std::string> requiredFiles;
}

std::string GetDataLocationFromString(const std::string& str)
{
    if (str.empty())
        return DEFAULT_DATA_PATH;

    if (str[str.size() - 1] != '/')
    {
        return str + '/';
    }

    return str;
}

std::string GetShaderLocationFromString(const std::string& str)
{
    if (str.empty())
        return DEFAULT_SHADERS_PATH;

    if (str[str.size() - 1] != '/')
    {
        return str + '/';
    }

    return str;
}

void AddToRequierdFilesIfNotExist(const std::string& file)
{
    if (std::find(requiredFiles.begin(), requiredFiles.end(), file) == requiredFiles.end())
    {
        requiredFiles.push_back(file);
    }
}

std::string GetFullDataPath(const std::string& fileName, bool addToRequierd)
{
    auto inputFileName = Utils::ReplaceSlash(fileName);

    std::string path;
    if (Utils::IsAbsolutePath(inputFileName))
    {
        path = inputFileName;
    }
    else
    {
        path = EngineConf.files.data + inputFileName;
    }

    if (addToRequierd)
    {
        AddToRequierdFilesIfNotExist(inputFileName);
    }
    return path;
}

std::string GetFullShaderPath(const std::string& file_name, bool addToRequierd)
{
    auto path = EngineConf.files.shaders + file_name;
    if (addToRequierd)
        requiredFiles.push_back(path);
    return path;
}

std::string GetFilePatch(const std::string& file_full_path)
{
    if (file_full_path.empty())
        return {};

    auto data_index = file_full_path.find(EngineConf.files.data);
    size_t size     = EngineConf.files.data.size();

    if (data_index == std::string::npos)
    {
        data_index = file_full_path.find(EngineConf.files.shaders);
        size       = EngineConf.files.shaders.size();
    }
    if (data_index == std::string::npos)
    {
        return file_full_path;
    }
    return file_full_path.substr(data_index + size);
}

void AddRequiredFile(const std::string& file)
{
    AddToRequierdFilesIfNotExist(file);
}

void SaveRequiredFiles()
{
    std::ofstream output(GetFullDataPath(EngineConf.files.requiredFilesOutputFile));

    if (!output.is_open())
    {
        ERROR_LOG("Cant open file : " + GetFullDataPath(EngineConf.files.requiredFilesOutputFile));
        return;
    }

    for (const auto& str : requiredFiles)
    {
        output << str << std::endl;
    }
    output.close();
}

void ReadFromFile(const std::string& filename)
{
    if (Utils::CheckFileExist(filename))
    {
        ReadConfiguration(EngineConf, filename);
    }
    else
    {
        auto parentPath = std::filesystem::path(filename).parent_path();
        if (not std::filesystem::exists(parentPath))
        {
            std::filesystem::create_directories(parentPath);
        }
    }

    WriteConfigurationToFile(EngineConf, filename);
    AddRequiredFile(filename);
    EngineConf.filename = filename;
}
std::string RemoveDataPath(const std::string& path)
{
    return GetFilePatch(path);
}

std::string GetRelativeDataPath(const std::string& str)
{
    auto absoluteDataPath = Utils::GetAbsolutePath(EngineConf.files.data);
    return Utils::GetRelativePath(str, absoluteDataPath);
}

Configuration::Configuration()
{
}

Params::DebugParams::DebugParams()
    : logLvl{LogginLvl::None,
             {LogginLvl::None, LogginLvl::Error, LogginLvl::ErrorWarning, LogginLvl::ErrorWarningInfo,
              LogginLvl::ErrorWarningInfoDebug}}
    , showRenderInfo{false, {false, true}}
    , showPhycicsVisualization{false, {false, true}}
{
}

}  // namespace GameEngine
