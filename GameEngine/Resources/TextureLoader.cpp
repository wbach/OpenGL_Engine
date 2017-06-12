#include "TextureLoader.h"
#include "OpenGLLoader.h"
#include "Textures/MaterialTexture.h"
#include "Textures/CubeMapTexture.h"
#include "../Debug_/Log.h"
#include "../Engine/Configuration.h"
#include <FreeImage.h>
#include <algorithm>

CTextureLoader::CTextureLoader(std::vector<std::unique_ptr<CTexture>>& textures_vector, COpenGLLoader & openGLLoader)
    : textures(textures_vector)
    , openGLLoader(openGLLoader)
{
}

void CTextureLoader::ReadFile(const std::string & file, SImage& image, bool applySizeLimit, TextureFlip::Type flip_mode)
{
    auto file_location = file;
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file_location.c_str(), 0);
	if (formato == FIF_UNKNOWN)
	{
        Log("[Error] GetFileType: wrong image format or file does not exist : " + file_location);
		return;
	}

    FIBITMAP* imagen2 = FreeImage_Load(formato, file_location.c_str());
	if (!imagen2)
	{
        Log("[Error] FreeImageLoad: wrong image format or file does not exist : " + file_location);
		return;
	}

	FIBITMAP* imagen = FreeImage_ConvertTo32Bits(imagen2);
	if (!imagen)
	{
		FreeImage_Unload(imagen2);
        Log("[Error] Cant convert to 32 bits : " + file_location);
		return;
	}

    if (flip_mode == TextureFlip::Type::VERTICAL || flip_mode == TextureFlip::Type::BOTH)
        FreeImage_FlipVertical(imagen);
    if (flip_mode == TextureFlip::Type::HORIZONTAL || flip_mode == TextureFlip::Type::BOTH)
        FreeImage_FlipHorizontal(imagen);

	int w = FreeImage_GetWidth(imagen);
	int h = FreeImage_GetHeight(imagen);	
	
	if (applySizeLimit)
	{
        auto& texture_size = EngineConf.maxTextureResolutuion;
		bool resize_texture = false;

		if (w > texture_size.x)
		{
			w = texture_size.x;
			resize_texture = true;
		}
		if (h > texture_size.y)
		{
			h = texture_size.y;
			resize_texture = true;
		}
		if (resize_texture)
			imagen = FreeImage_Rescale(imagen, w, h, FILTER_BSPLINE);
	}

    image.width = w;
    image.height = h;

	char* pixeles = (char*)FreeImage_GetBits(imagen);

    image.data = new GLubyte[4 * w*h];
	//bgr2rgb
	for (int j = 0; j<w*h; j++)
	{
        image.data[j * 4 + 0] = pixeles[j * 4 + 2];
        image.data[j * 4 + 1] = pixeles[j * 4 + 1];
        image.data[j * 4 + 2] = pixeles[j * 4 + 0];
        image.data[j * 4 + 3] = pixeles[j * 4 + 3];
	}
	FreeImage_Unload(imagen);
	FreeImage_Unload(imagen2);
    Log("File: " + file_location + " is loaded.");
}

CTexture* CTextureLoader::LoadTexture(const std::string & file, bool applySizeLimit, bool opengl_pass, TextureType::Type type, TextureFlip::Type flip_mode)
{
    for (auto& t : textures)
        if (t->GetFileName() == file)
            return t.get();

	SImage texture; 
	ReadFile(EngineConf_GetFullDataPathAddToRequierd(file), texture, applySizeLimit, flip_mode);

	switch (type)
	{
	case TextureType::MATERIAL:
         textures.emplace_back(new CMaterialTexture(false, file, file, texture));
		break;
	}
	if(opengl_pass)
        openGLLoader.AddObjectToOpenGLLoadingPass(textures.back().get());
    return textures.back().get();
}

CTexture * CTextureLoader::LoadTextureImmediately(const std::string & file, bool applySizeLimit, TextureType::Type type, TextureFlip::Type flip_mode)
{
	auto texture = LoadTexture(file, applySizeLimit, false, type, flip_mode);
	texture->OpenGLLoadingPass();
	return texture;
}

CTexture * CTextureLoader::LoadCubeMap(std::vector<std::string>& files, bool applySizeLimit, bool opengl_pass)
{
	if (files.size() != 6)
	{
		Log("Cube map texture need 6 texutres.");
		return nullptr;
	}

	std::vector<SImage> images;
	images.resize(6);

	int x = 0;
	for (const auto& file : files)
        ReadFile(EngineConf_GetFullDataPathAddToRequierd(file), images[x++], applySizeLimit, TextureFlip::Type::VERTICAL);

    textures.emplace_back(new CCubeMapTexture(files[0], images));

    auto texture = textures.back().get();

	if(opengl_pass)
        openGLLoader.AddObjectToOpenGLLoadingPass(texture);

	return texture;
}
