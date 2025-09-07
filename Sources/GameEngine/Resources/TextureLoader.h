#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <Mutex.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ITextureLoader.h"
#include "ResourceInfo.h"
#include "Textures/TextureFlip.h"

namespace GameEngine
{
class TextureLoader : public ITextureLoader
{
public:
    TextureLoader(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&);
    ~TextureLoader() override;

    GeneralTexture* CreateTexture(const std::string&, const TextureParameters&, const Utils::Image&) override;
    void UpdateTexture(const GeneralTexture&) override;
    void UpdateTexture(GeneralTexture*&, const std::string&) override;
    GeneralTexture* LoadTexture(const File&, const TextureParameters&) override;
    GeneralTexture* LoadTexture(const std::string&, const unsigned char*, unsigned int, const TextureParameters&) override;
    CubeMapTexture* LoadCubeMap(const std::array<File, 6>&, const TextureParameters&) override;
    GeneralTexture* CreateNormalMap(const HeightMap&, const vec3&) override;
    HeightMap* CreateHeightMap(const File&, const vec2ui&, const TextureParameters&) override;
    HeightMap* LoadHeightMap(const File&, const TextureParameters&) override;
    GraphicsApi::IGraphicsApi& GetGraphicsApi() override;
    void DeleteTexture(Texture& texture) override;
    void LockReleaseResources() override;
    void UnlockReleaseResources() override;

private:
    HeightMap* LoadHeightMapBinary(const File&, const TextureParameters&);
    HeightMap* LoadHeightMapTexture(const File&, const TextureParameters&);
    Texture* GetTextureIfLoaded(const std::string&, const TextureParameters&);
    GeneralTexture* GetTextureNotFound();

    void AddTexture(const std::string& name, std::unique_ptr<Texture>, TextureLoadType);
    void ApplyLoadTypeAction(Texture&, TextureLoadType);
    std::string GetNoName() const;
    ResourceGpuStatus Convert(TextureLoadType);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuResourceLoader_;
    std::unordered_map<std::string, ResourceInfo<Texture>> textures_;
    std::pair<GeneralTexture*, bool> textureNotFound_;
    std::mutex textureMutex_;
    bool releaseLockState_;

    struct MemmoryImage
    {
        unsigned char* data;
        unsigned int length;
    };

    std::unordered_map<std::string, MemmoryImage> memoryImages;
};
}  // namespace GameEngine
