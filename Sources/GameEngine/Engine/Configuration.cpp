#include "Configuration.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>

namespace GameEngine
{
namespace Params
{
DebugParams::DebugParams()
    : logLvl{LoggingLvl::None,
             {LoggingLvl::None, LoggingLvl::Error, LoggingLvl::ErrorWarning, LoggingLvl::ErrorWarningInfo,
              LoggingLvl::ErrorWarningInfoDebug}}
    , showRenderInfo{false, {false, true}}
    , showRenderersTimers{false, {false, true}}
    , showPhycicsVisualization{false, {false, true}}
{
}

const std::filesystem::path& Files::getGeneratedDirPath() const
{
    return generated;
}
const std::filesystem::path& Files::getCacheDirPath() const
{
    return cache;
}
void Files::setLoadingBackgroundPath(const std::filesystem::path& newPath)
{
    loadingScreenBackgroundTexture = newPath;
}
void Files::setLoadingCirclePath(const std::filesystem::path& newPath)
{
    loadingScreenCircleTexture = newPath;
}
void Files::setShaderPath(const std::filesystem::path& newPath)
{
    shaders = newPath;
}
void Files::setDataPath(const std::filesystem::path& newPath)
{
    data = newPath;

    cache     = data / ".cache";
    generated = data / "generated";

    if (not std::filesystem::exists(data))
        Utils::CreateDirectories(data);
    if (not std::filesystem::exists(cache))
        Utils::CreateDirectories(cache);
    if (not std::filesystem::exists(generated))
        Utils::CreateDirectories(generated);
}
const std::filesystem::path& Files::getDataPath() const
{
    return data;
}
const std::filesystem::path& Files::getShaderPath() const
{
    return shaders;
}
const std::filesystem::path& Files::getLoadingCirclePath() const
{
    return loadingScreenCircleTexture;
}
const std::filesystem::path& Files::getLoadingBackgroundPath() const
{
    return loadingScreenBackgroundTexture;
}
}  // namespace Params
}  // namespace GameEngine
