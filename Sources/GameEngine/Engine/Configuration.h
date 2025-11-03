#pragma once
#include <SingleTon.h>

#include "ConfigurationParams/ConfigurationParam.h"
#include "ConfigurationParams/DebugParams/LineMeshVisualizatorParams.h"
#include "ConfigurationParams/RendererParams/RendererParams.h"
#include "ConfigurationParams/SoundParams/SoundParams.h"
#include "ConfigurationParams/WindowParams/WindowParams.h"
#include "LoD.h"

namespace GameEngine
{
namespace Params
{
class Files
{
public:
    const std::filesystem::path& getDataPath() const;
    const std::filesystem::path& getShaderPath() const;
    const std::filesystem::path& getGeneratedDirPath() const;
    const std::filesystem::path& getCacheDirPath() const;
    const std::filesystem::path& getLoadingCirclePath() const;
    const std::filesystem::path& getLoadingBackgroundPath() const;

    void setDataPath(const std::filesystem::path&);
    void setShaderPath(const std::filesystem::path&);
    void setLoadingCirclePath(const std::filesystem::path&);
    void setLoadingBackgroundPath(const std::filesystem::path&);

private:
    std::filesystem::path data{"../Data/"};
    std::filesystem::path shaders{"../Sources/"};
    std::filesystem::path loadingScreenCircleTexture{std::filesystem::path("GUI") / "aLoadingCircle.png"};
    std::filesystem::path loadingScreenBackgroundTexture{std::filesystem::path("GUI") / "dark3.png"};
    std::filesystem::path cache;
    std::filesystem::path generated;
};

struct DebugParams
{
    DebugParams();
    LineMeshVisualizatorParams linemeshVisualizator;
    Params::ConfigurationParam<LoggingLvl> logLvl;
    Params::ConfigurationParam<bool> showRenderInfo;
    Params::ConfigurationParam<bool> showPhycicsVisualization;
};
}  // namespace Params

struct Configuration
{
    std::string filename;
    Params::Files files;
    Params::Sound sound;
    Params::Window window;
    Params::Renderer renderer;
    Params::DebugParams debugParams;
    bool useBinaryLoading = false;
};
}  // namespace GameEngine

#define EngineConf SingleTon<GameEngine::Configuration>::Get()
