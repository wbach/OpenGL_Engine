#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GameEngine/Api/IGraphicsApi.h"
#include "ITextureLoader.h"
#include "Textures/TextureFlip.h"

namespace GameEngine
{
class TextureLoader : public ITextureLoader
{
public:
    TextureLoader(IGraphicsApi& graphicsApi, std::vector<std::unique_ptr<Texture>>& textures_vector, std::shared_ptr<IGpuResourceLoader> gpuLoader);
    ~TextureLoader() override;
    std::optional<Image> ReadFile(const std::string& file, bool applySizeLimit = true, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override;
    // Return place in texture loader not in OpenGL
    Texture* LoadTexture(const std::string& file, bool applySizeLimit = true, bool gpu_pass = true, ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override;
    Texture* LoadTextureImmediately(const std::string& file, bool applySizeLimit = true, ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override;
    Texture* LoadCubeMap(const std::vector<std::string>& files, bool applySizeLimit = true, bool gpu_pass = true) override;
    Texture* LoadHeightMap(const std::string& filename, bool gpu_pass = true) override;
    void CreateHeightMap(const std::string& input, const std::string& output) override;
    IGraphicsApi& GetGraphicsApi() override;
    
private:
    Texture* GetTextureIfLoaded(const std::string& filename) const;
    std::optional<uint32> GetNumberOfRowsBasedOnTextureFileName(const std::string&) const;

private:
    IGraphicsApi& graphicsApi_;
    std::vector<std::unique_ptr<Texture>>& textures_;
    std::shared_ptr<IGpuResourceLoader> gpuResourceLoader_;
};
}  // namespace GameEngine
