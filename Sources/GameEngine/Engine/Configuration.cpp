#include "Configuration.h"
#include <fstream>
#include "ConfigurationReader.h"
#include "ConfigurationWriter.h"
#include "Logger/Log.h"

namespace GameEngine
{
std::vector<std::string> requiredFiles;

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
std::string GetFullDataPath(const std::string& file_name, bool addToRequierd)
{
    auto path = EngineConf.files.data + file_name;
    if (addToRequierd)
    {
        if (std::find(requiredFiles.begin(), requiredFiles.end(), file_name) != requiredFiles.end())
        {
            requiredFiles.push_back(path);
        }
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
    uint32 size     = EngineConf.files.data.size();

    if (data_index == std::string::npos)
    {
        data_index = file_full_path.find(EngineConf.files.shaders);
        size       = EngineConf.files.shaders.size();
    }

    auto str = file_full_path.substr(data_index + size);

    return str;
}

void AddRequiredFile(const std::string& file)
{
    requiredFiles.push_back(file);
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
        WriteConfigurationToFile(EngineConf, filename);
    }
    AddRequiredFile(filename);
}
}  // namespace GameEngine
