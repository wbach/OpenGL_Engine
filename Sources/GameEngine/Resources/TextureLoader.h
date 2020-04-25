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

    std::optional<Image> ReadFile(const InputFileName& file, const TextureParameters&);
    Texture* CreateTexture(const std::string&, const TextureParameters&, const TextureSize&, RawData) override;
    Texture* LoadTexture(const InputFileName&, const TextureParameters&) override;
    Texture* LoadCubeMap(const std::array<InputFileName, 6>&, const TextureParameters&) override;
    Texture* LoadNormalMap(const std::vector<float>&, const TextureSize&, float) override;
    Texture* LoadHeightMap(const InputFileName&, const TextureParameters&) override;
    void CreateHeightMap(const std::string& input, const std::string& output, const vec3&) override;
    void SetHeightMapFactor(float) override;
    GraphicsApi::IGraphicsApi& GetGraphicsApi() override;
    void SaveTextureToFile(const std::string& name, const std::vector<uint8>&, const vec2ui& size, uint8 bytes, GraphicsApi::TextureFormat) const override;
    void DeleteTexture(Texture& texture) override;

private:
    Texture* LoadHeightMapBinary(const InputFileName&, const TextureParameters&);
    Texture* LoadHeightMapTexture(const InputFileName&, const TextureParameters&);
    Texture* GetTextureIfLoaded(const std::string&);
    Texture* GetTextureNotFound();

    Texture* AddTexture(const std::string& name, std::unique_ptr<Texture>, TextureLoadType);
    void ApplyLoadTypeAction(Texture&, TextureLoadType);
    std::string GetNoName() const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuResourceLoader_;
    std::unordered_map<std::string, ResourceInfo<Texture>>& textures_;
    std::pair<Texture*, bool> textureNotFound_;
    float heightMapFactor_;
    std::mutex textureMutex_;
};
}  // namespace GameEngine
