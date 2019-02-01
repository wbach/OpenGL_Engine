#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GraphicsApi/IGraphicsApi.h"
#include "ITextureLoader.h"
#include "Textures/TextureFlip.h"

namespace GameEngine
{
class TextureLoader : public ITextureLoader
{
public:
    TextureLoader(GraphicsApi::IGraphicsApi& graphicsApi, std::vector<std::unique_ptr<Texture>>& textures_vector, std::shared_ptr<IGpuResourceLoader> gpuLoader);
    ~TextureLoader() override;
    std::optional<Image> ReadFile(const std::string& file, bool applySizeLimit = true, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override;
    // Return place in texture loader not in OpenGL
    Texture* CreateTexture(const std::string& name, GraphicsApi::TextureType type, GraphicsApi::TextureFilter filter, GraphicsApi::TextureMipmap mimpamp, GraphicsApi::BufferAtachment atachment, vec2ui size, void* data) override;
    Texture* LoadTexture(const std::string& file, bool applySizeLimit = true, bool gpu_pass = true, ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override;
    Texture* LoadTextureImmediately(const std::string& file, bool applySizeLimit = true, ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override;
    Texture* LoadCubeMap(const std::vector<std::string>& files, bool applySizeLimit = true, bool gpu_pass = true) override;
    Texture* LoadHeightMap(const std::string& filename, bool gpu_pass = true) override;
    void CreateHeightMap(const std::string& input, const std::string& output) override;
    virtual void SetHeightMapFactor(float) override;
    GraphicsApi::IGraphicsApi& GetGraphicsApi() override;
    
private:
    Texture* GetTextureIfLoaded(const std::string& filename) const;
    std::optional<uint32> GetNumberOfRowsBasedOnTextureFileName(const std::string&) const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::vector<std::unique_ptr<Texture>>& textures_;
    std::shared_ptr<IGpuResourceLoader> gpuResourceLoader_;
    float heightMapFactor_;
};
}  // namespace GameEngine
