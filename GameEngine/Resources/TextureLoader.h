#pragma once
#include "Textures/TextureFlip.h"
#include <string>
#include <vector>
#include <memory>

class CTexture;
class COpenGLLoader;
struct SImage;

namespace TextureType
{
	enum Type
	{
		MATERIAL = 0
	};
}

class CTextureLoader
{
public:
	CTextureLoader(std::vector<std::unique_ptr<CTexture>>& textures_vector, COpenGLLoader& openGLLoader);
	void ReadFile(const std::string& file, SImage& image, bool applySizeLimit = true, TextureFlip::Type flip_mode = TextureFlip::NONE);
	//Return place in texture loader not in OpenGL
	CTexture* LoadTexture(const std::string& file, bool applySizeLimit = true, bool opengl_pass = true, TextureType::Type type = TextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::NONE);
	CTexture* LoadTextureImmediately(const std::string& file, bool applySizeLimit = true, TextureType::Type type = TextureType::MATERIAL, TextureFlip::Type flip_mode = TextureFlip::NONE);
	CTexture* LoadCubeMap(std::vector<std::string>& files, bool applySizeLimit = true, bool opengl_pass = true);
private:
    std::vector<std::unique_ptr<CTexture>>& textures;
    COpenGLLoader&   openGLLoader;
};
