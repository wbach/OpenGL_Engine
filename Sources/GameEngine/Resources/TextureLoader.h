#pragma once
#include <memory>
#include <string>
#include <vector>
#include <GraphicsApi/IGraphicsApi.h>
#include "ITextureLoader.h"
#include "Textures/TextureFlip.h"
#include "ResourceInfo.h"
#include <Mutex.hpp>

namespace GameEngine
{
class TextureLoader : public ITextureLoader
{
public:
    TextureLoader(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, std::unordered_map<std::string, ResourceInfo<Texture>>&);
    ~TextureLoader() override;

    Texture* CreateTexture(const std::string&, const TextureParameters&, const TextureSize&, RawData) override;
    Texture* LoadTexture(const File&, const TextureParameters&) override;
    Texture* LoadCubeMap(const std::array<File, 6>&, const TextureParameters&) override;
    Texture* CreateNormalMap(const HeightMap&, const vec3&) override;
    Texture* LoadHeightMap(const File&, const TextureParameters&) override;
    GraphicsApi::IGraphicsApi& GetGraphicsApi() override;
    void SaveTextureToFile(const File&, const std::vector<uint8>&, const vec2ui& size, uint8 bytes, GraphicsApi::TextureFormat) const override;
    void DeleteTexture(Texture& texture) override;

private:
    Texture* LoadHeightMapBinary(const File&, const TextureParameters&);
    Texture* LoadHeightMapTexture(const File&, const TextureParameters&);
    Texture* GetTextureIfLoaded(const std::string&, const TextureParameters&);
    Texture* GetTextureNotFound();

    Texture* AddTexture(const std::string& name, std::unique_ptr<Texture>, TextureLoadType);
    void ApplyLoadTypeAction(Texture&, TextureLoadType);
    std::string GetNoName() const;
    ResourceGpuStatus Convert(TextureLoadType);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuResourceLoader_;
    std::unordered_map<std::string, ResourceInfo<Texture>>& textures_;
    std::pair<Texture*, bool> textureNotFound_;
    std::mutex textureMutex_;
};
}  // namespace GameEngine
