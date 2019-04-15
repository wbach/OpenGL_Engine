#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "GraphicsApi/IGraphicsApi.h"
#include "Textures/TextureFlip.h"

namespace GameEngine
{
class Texture;
class IGpuResourceLoader;
struct Image;

enum class ObjectTextureType
{
    MATERIAL = 0,
    HEIGHT_MAP
};

class ITextureLoader
{
public:
    virtual ~ITextureLoader() = default;

    virtual std::optional<Image> ReadFile(const std::string& file, bool applySizeLimit = true,
                                          TextureFlip::Type flip_mode = TextureFlip::Type::NONE) = 0;
    // Return place in texture loader not in OpenGL
    virtual Texture* CreateTexture(const std::string& name, GraphicsApi::TextureType type,
                                   GraphicsApi::TextureFilter filter, GraphicsApi::TextureMipmap mimpamp,
                                   GraphicsApi::BufferAtachment atachment, vec2ui size, void* data)        = 0;
    virtual Texture* LoadTexture(const std::string& file, bool applySizeLimit = true, bool gpu_pass = true,
                                 ObjectTextureType type      = ObjectTextureType::MATERIAL,
                                 TextureFlip::Type flip_mode = TextureFlip::Type::NONE)                    = 0;
    virtual Texture* LoadTextureImmediately(const std::string& file, bool applySizeLimit = true,
                                            ObjectTextureType type      = ObjectTextureType::MATERIAL,
                                            TextureFlip::Type flip_mode = TextureFlip::Type::NONE)         = 0;
    virtual Texture* LoadCubeMap(const std::vector<std::string>& files, bool applySizeLimit = true,
                                 bool gpu_pass = true)                                                     = 0;
    virtual Texture* LoadHeightMap(const std::string& filename, bool gpu_pass = true)                      = 0;
    virtual Texture* LoadNormalMap(const std::vector<float>& baseData, const vec2ui& size, float strength) = 0;
    virtual void CreateHeightMap(const std::string& input, const std::string& output)                      = 0;
    virtual void SetHeightMapFactor(float)                                                                 = 0;
    virtual GraphicsApi::IGraphicsApi& GetGraphicsApi()                                                    = 0;
    virtual void SaveTextureToFile(const std::string& name, const std::vector<uint8>&, const vec2ui& size, uint8 bytes,
                                   GraphicsApi::TextureFormat) const                                       = 0;
};
}  // namespace GameEngine
