#include "ConfigurationReader.h"

#include <Logger/Log.h>
#include <sys/types.h>

#include <filesystem>
#include <iostream>

#include "Configuration.h"
#include "EngineDef.h"

#ifdef USE_GNU
#include <pwd.h>
#include <unistd.h>
#else
#include <Utils/Windows.hpp>
#endif

#include <magic_enum/magic_enum.hpp>

namespace GameEngine
{
std::string getConfigFile()
{
    std::string configFile("./Conf.xml");
    if (std::filesystem::exists(configFile))
        return configFile;

    configFile = "./config.xml";
    if (std::filesystem::exists(configFile))
        return configFile;

#ifdef USE_GNU
    struct passwd* pw = getpwuid(getuid());
    configFile        = std::string(pw->pw_dir) + "/.config/bengine/Conf.xml";
#else
    wchar_t myDocumentsPath[1024];
    HRESULT hr = SHGetFolderPathW(0, CSIDL_MYDOCUMENTS, 0, 0, myDocumentsPath);
    if (SUCCEEDED(hr))
    {
        char str[1024];
        wcstombs(str, myDocumentsPath, 1023);
        configFile = std::string(str) + "\\bengine\\Conf.xml";
    }
#endif
    return configFile;
}

ConfigurationReader::ConfigurationReader()
    : ConfigurationReader(getConfigFile())
{
}
ConfigurationReader::ConfigurationReader(const std::string& filename)
{
    LOG_DEBUG << "Read config file: " << filename;
    GameEngine::ReadFromFile(filename);

    if (EngineConf.debugParams.logLvl != LoggingLvl::None)
    {
        CLogger::Instance().EnableLogs(EngineConf.debugParams.logLvl);
        CLogger::Instance().UseAsyncLogging(false);
        std::cout << "LoggingLvl: " << magic_enum::enum_name(EngineConf.debugParams.logLvl.get()) << std::endl;
    }
}
}  // namespace GameEngine
