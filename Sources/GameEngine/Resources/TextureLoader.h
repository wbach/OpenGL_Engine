#pragma once
#include "Textures/TextureFlip.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include <memory>
#include <string>
#include <vector>

class CTexture;
class COpenGLLoader;
struct SImage;


enum class ObjectTextureType
{
    MATERIAL = 0,
	HEIGHT_MAP
};

class CTextureLoader
{
public:
    CTextureLoader(GameEngine::IGraphicsApiPtr graphicsApi, std::vector<std::unique_ptr<CTexture>>& textures_vector, COpenGLLoader& openGLLoader);
    void ReadFile(const std::string& file, SImage& image, bool applySizeLimit = true, TextureFlip::Type flip_mode = TextureFlip::Type::NONE);
    // Return place in texture loader not in OpenGL
    CTexture* LoadTexture(const std::string& file, bool applySizeLimit = true, bool opengl_pass = true,
		ObjectTextureType type = ObjectTextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::Type::NONE);
    CTexture* LoadTextureImmediately(const std::string& file, bool applySizeLimit = true, ObjectTextureType type = ObjectTextureType::MATERIAL,
                                     TextureFlip::Type flip_mode = TextureFlip::Type::NONE);
    CTexture* LoadCubeMap(std::vector<std::string>& files, bool applySizeLimit = true, bool opengl_pass = true);
	CTexture* LoadHeightMap(const std::string& filename, bool opengl_pass = true);
	void CreateHeightMap(const std::string& input, const std::string& output);
	GameEngine::IGraphicsApiPtr GetGraphicsApi();

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    std::vector<std::unique_ptr<CTexture>>& textures;
    COpenGLLoader& openGLLoader;
};
