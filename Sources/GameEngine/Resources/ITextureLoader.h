#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "File.h"
#include "TextureParameters.h"

namespace GameEngine
{
class Texture;
class HeightMap;
class GeneralTexture;
class CubeMapTexture;
class IGpuResourceLoader;

class ITextureLoader
{
public:
    virtual ~ITextureLoader() = default;

    virtual GeneralTexture* CreateTexture(const std::string&, const TextureParameters&, const Utils::Image&) = 0;
    virtual void UpdateTexture(const GeneralTexture&)                                                        = 0;
    virtual void UpdateTexture(GeneralTexture*&, const std::string&)                                         = 0;
    virtual GeneralTexture* LoadTexture(const File&, const TextureParameters&)                               = 0;
    virtual CubeMapTexture* LoadCubeMap(const std::array<File, 6>&, const TextureParameters&)                = 0;
    virtual HeightMap* LoadHeightMap(const File&, const TextureParameters&)                                  = 0;
    virtual HeightMap* CreateHeightMap(const File&, const vec2ui&, const TextureParameters&)                 = 0;
    virtual GeneralTexture* CreateNormalMap(const HeightMap&, const vec3&)                                   = 0;
    virtual GraphicsApi::IGraphicsApi& GetGraphicsApi()                                                      = 0;
    virtual void DeleteTexture(Texture&)                                                                     = 0;
    virtual void LockReleaseResources()                                                                      = 0;
    virtual void UnlockReleaseResources()                                                                    = 0;
};
}  // namespace GameEngine
