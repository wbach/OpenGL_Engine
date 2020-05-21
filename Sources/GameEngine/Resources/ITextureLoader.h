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
class IGpuResourceLoader;
class HeightMap;
struct Image;

class ITextureLoader
{
public:
    virtual ~ITextureLoader() = default;

    virtual Texture* CreateTexture(const std::string&, const TextureParameters&, const TextureSize&, RawData) = 0;
    virtual Texture* LoadTexture(const File&, const TextureParameters&)                                       = 0;
    virtual Texture* LoadCubeMap(const std::array<File, 6>&, const TextureParameters&)                        = 0;
    virtual Texture* LoadHeightMap(const File&, const TextureParameters&)                                     = 0;
    virtual Texture* CreateNormalMap(const HeightMap&, const vec3&)                                           = 0;
    virtual GraphicsApi::IGraphicsApi& GetGraphicsApi()                                                       = 0;
    virtual void SaveTextureToFile(const File&, const std::vector<uint8>&, const TextureSize&, uint8,
                                   GraphicsApi::TextureFormat) const                                          = 0;
    virtual void DeleteTexture(Texture&)                                                                      = 0;
};
}  // namespace GameEngine
