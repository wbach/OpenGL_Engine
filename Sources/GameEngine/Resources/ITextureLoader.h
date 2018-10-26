#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GameEngine/Api/IGraphicsApi.h"
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
    virtual ~ITextureLoader()
    {
    }
    virtual void ReadFile(const std::string& file, Image& image, bool applySizeLimit = true, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) = 0;
    // Return place in texture loader not in OpenGL
    virtual Texture* LoadTexture(const std::string& file, bool applySizeLimit = true, bool gpu_pass = true, ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) = 0;
    virtual Texture* LoadTextureImmediately(const std::string& file, bool applySizeLimit = true, ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) = 0;
    virtual Texture* LoadCubeMap(std::vector<std::string>& files, bool applySizeLimit = true, bool gpu_pass = true) = 0;
    virtual Texture* LoadHeightMap(const std::string& filename, bool gpu_pass = true) = 0;
    virtual void CreateHeightMap(const std::string& input, const std::string& output) = 0;
    virtual IGraphicsApiPtr GetGraphicsApi() = 0;
};
}  // namespace GameEngine
