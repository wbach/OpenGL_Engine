#include "ConfigurationReader.h"

#include <Logger/Log.h>

#include <iostream>

#include "Configuration.h"
#include "EngineDef.h"

#include <sys/types.h>
#ifdef GNU
#include <pwd.h>
#include <unistd.h>
#else
#include <Utils/Windows.hpp>
#endif

namespace GameEngine
{
ConfigurationReader::ConfigurationReader()
{
    std::string configFile("./Conf.xml");

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
    GameEngine::ReadFromFile(configFile);

    if (EngineConf.debugParams.logLvl != LogginLvl::None)
    {
        CLogger::Instance().EnableLogs(EngineConf.debugParams.logLvl);
        CLogger::Instance().ImmeditalyLog();
        std::cout << "LogginLvl: " << Params::paramToString(EngineConf.debugParams.logLvl) << std::endl;
    }
}

}  // namespace GameEngine
