#pragma once
#include <gmock/gmock.h>
#include "../Api/GraphicsApiMock.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "OpenGLLoaderMock.h"
#include "Resources/ITextureLoader.h"

namespace GameEngine
{
class TextureLoaderMock : public ITextureLoader
{
public:
    MOCK_METHOD3(ReadFileImpl, std::optional<Image>(const std::string&, bool, TextureFlip::Type));
    MOCK_METHOD5(LoadTextureImpl, Texture*(const std::string&, bool, bool, ObjectTextureType, TextureFlip::Type));
    MOCK_METHOD4(LoadTextureImmediatelyImpl, Texture*(const std::string&, bool, ObjectTextureType, TextureFlip::Type));
    MOCK_METHOD3(LoadCubeMapImpl, Texture*(const std::vector<std::string>&, bool, bool));
    MOCK_METHOD2(LoadHeightMapImpl, Texture*(const std::string&, bool));
    MOCK_METHOD2(CreateHeightMap, void(const std::string&, const std::string&));
    MOCK_METHOD0(GetGraphicsApi, GraphicsApi::IGraphicsApi&());
    MOCK_METHOD7(CreateTextureImpl, Texture*(const std::string&, GraphicsApi::TextureType, GraphicsApi::TextureFilter,
                                             GraphicsApi::TextureMipmap, GraphicsApi::BufferAtachment, vec2ui, void*));
    MOCK_METHOD3(LoadNormalMapImpl, Texture*(const std::vector<float>&, const vec2ui&, float));
    MOCK_METHOD1(SetHeightMapFactorImpl, void(float));
    MOCK_CONST_METHOD5(SetTextureToFileImpl, void(const std::string&, const std::vector<uint8>&, const vec2ui&, uint8,
                                                  GraphicsApi::TextureFormat));

private:
    std::optional<Image> ReadFile(const std::string& file, bool applySizeLimit = true,
                                  TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override
    {
        return ReadFileImpl(file, applySizeLimit, flip_mode);
    }

    Texture* LoadTexture(const std::string& file, bool applySizeLimit = true, bool gpu_pass = true,
                         ObjectTextureType type      = ObjectTextureType::MATERIAL,
                         TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override
    {
        return LoadTextureImpl(file, applySizeLimit, gpu_pass, type, flip_mode);
    }

    Texture* LoadTextureImmediately(const std::string& file, bool applySizeLimit = true,
                                    ObjectTextureType type      = ObjectTextureType::MATERIAL,
                                    TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override
    {
        return LoadTextureImmediatelyImpl(file, applySizeLimit, type, flip_mode);
    }

    Texture* LoadCubeMap(const std::vector<std::string>& files, bool applySizeLimit = true,
                         bool gpu_pass = true) override
    {
        return LoadCubeMapImpl(files, applySizeLimit, gpu_pass);
    }

    Texture* LoadHeightMap(const std::string& filename, bool gpu_pass = true) override
    {
        return LoadHeightMapImpl(filename, gpu_pass);
    }
    Texture* CreateTexture(const std::string& name, GraphicsApi::TextureType type, GraphicsApi::TextureFilter filter,
                           GraphicsApi::TextureMipmap mimpamp, GraphicsApi::BufferAtachment atachment, vec2ui size,
                           void* data) override
    {
        return CreateTextureImpl(name, type, filter, mimpamp, atachment, size, data);
    }
    Texture* LoadNormalMap(const std::vector<float>& baseData, const vec2ui& size, float strength) override
    {
        return LoadNormalMapImpl(baseData, size, strength);
    }
    void SetHeightMapFactor(float f) override
    {
        SetHeightMapFactorImpl(f);
    }
    void SaveTextureToFile(const std::string& name, const std::vector<uint8>& data, const vec2ui& size, uint8 bytes,
                           GraphicsApi::TextureFormat foramt) const override
    {
        SetTextureToFileImpl(name, data, size, bytes, foramt);
    }
};
}  // namespace GameEngine
