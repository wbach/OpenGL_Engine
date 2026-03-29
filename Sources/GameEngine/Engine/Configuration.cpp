#include "Configuration.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>

namespace GameEngine
{
namespace Params
{
DebugParams::DebugParams()
    : logLvl{LoggingLvl::None,
             {LoggingLvl::None, LoggingLvl::Error, LoggingLvl::ErrorWarning, LoggingLvl::ErrorWarningInfo,
              LoggingLvl::ErrorWarningInfoDebug}}
    , generateDebugNormalsMeshes{false, {false, true}}
    , showRenderInfo{false, {false, true}}
    , showRenderersTimers{false, {false, true}}
    , showPhycicsVisualization{false, {false, true}}
    , visualizationTerrainShapeInPhysicsDebugDraw{false, {false, true}}
    , debugRendererDistance{25.f, {-1.f, 10.f, 25.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f}}
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
    makeAbolutePath(loadingScreenBackgroundTexture);
}
void Files::setLoadingBackgroundAudioPath(const std::filesystem::path& newPath)
{
    loadingScreenBackgroundAudio = newPath;
    makeAbolutePath(loadingScreenBackgroundAudio);
}
void Files::setLoadingCirclePath(const std::filesystem::path& newPath)
{
    loadingScreenCircleTexture = newPath;
    makeAbolutePath(loadingScreenCircleTexture);
}
void Files::setQuestsFile(const std::filesystem::path& newPath)
{
    quests = newPath;
    makeAbolutePath(quests);
}
void Files::setShaderPath(const std::filesystem::path& newPath)
{
    shaders = newPath;
    makeAbolutePath(shaders);
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

    makeAbolutePath(data);
    makeAbolutePath(cache);
    makeAbolutePath(generated);
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
const std::filesystem::path& Files::getQuestsFilePath() const
{
    return quests;
}
const std::filesystem::path& Files::getLoadingBackgroundPath() const
{
    return loadingScreenBackgroundTexture;
}
const std::filesystem::path& Files::getLoadingBackgroundAudioPath() const
{
    return loadingScreenBackgroundAudio;
}
void Files::setProjectPath(const std::filesystem::path& path)
{
    projectPath = path;

    makeAbolutePath(data);
    makeAbolutePath(cache);
    makeAbolutePath(generated);
    makeAbolutePath(shaders);
    makeAbolutePath(loadingScreenCircleTexture);
    makeAbolutePath(loadingScreenBackgroundTexture);
    makeAbolutePath(loadingScreenBackgroundAudio);
}
std::filesystem::path Files::getRelativeIfCan(const std::filesystem::path& input) const
{
    if (projectPath.empty())
        return input;

    auto rel = std::filesystem::relative(input, projectPath).lexically_normal();

    if (not rel.empty() and *rel.begin() != "..")
    {
        return rel;
    }

    return input;
}
const std::filesystem::path& Files::getProjectPath() const
{
    return projectPath;
}
void Files::makeAbolutePath(std::filesystem::path& input)
{
    if (projectPath.empty() or input.is_absolute())
        return;

    input = std::filesystem::absolute(projectPath / input).lexically_normal();
}
}  // namespace Params
}  // namespace GameEngine
