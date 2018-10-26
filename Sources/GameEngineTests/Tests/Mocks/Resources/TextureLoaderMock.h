#pragma once
#include <gmock/gmock.h>
#include "../Api/GraphicsApiMock.h"
#include "OpenGLLoaderMock.h"
#include "Resources/ITextureLoader.h"

namespace GameEngine
{
class TextureLoaderMock : public ITextureLoader
{
public:
    MOCK_METHOD4(ReadFileImpl, void(const std::string&, Image& image, bool, TextureFlip::Type));
    MOCK_METHOD5(LoadTextureImpl, Texture*(const std::string&, bool, bool, ObjectTextureType, TextureFlip::Type));
    MOCK_METHOD4(LoadTextureImmediatelyImpl, Texture*(const std::string&, bool, ObjectTextureType, TextureFlip::Type));
    MOCK_METHOD3(LoadCubeMapImpl, Texture*(std::vector<std::string>&, bool, bool));
    MOCK_METHOD2(LoadHeightMapImpl, Texture*(const std::string&, bool));
    MOCK_METHOD2(CreateHeightMap, void(const std::string&, const std::string&));
    MOCK_METHOD0(GetGraphicsApi, IGraphicsApiPtr());

private:
    void ReadFile(const std::string& file, Image& image, bool applySizeLimit = true, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override
    {
        ReadFileImpl(file, image, applySizeLimit, flip_mode);
    }

    Texture* LoadTexture(const std::string& file, bool applySizeLimit = true, bool gpu_pass = true, ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override
    {
        return LoadTextureImpl(file, applySizeLimit, gpu_pass, type, flip_mode);
    }

    Texture* LoadTextureImmediately(const std::string& file, bool applySizeLimit = true, ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE) override
    {
        return LoadTextureImmediatelyImpl(file, applySizeLimit, type, flip_mode);
    }

    Texture* LoadCubeMap(std::vector<std::string>& files, bool applySizeLimit = true, bool gpu_pass = true) override
    {
        return LoadCubeMapImpl(files, applySizeLimit, gpu_pass);
    }

    Texture* LoadHeightMap(const std::string& filename, bool gpu_pass = true) override
    {
        return LoadHeightMapImpl(filename, gpu_pass);
    }
};
}  // namespace GameEngine
