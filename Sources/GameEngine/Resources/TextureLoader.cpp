#define _CRT_SECURE_NO_WARNINGS
#include "TextureLoader.h"
#include "OpenGLLoader.h"
#include "Textures/MaterialTexture.h"
#include "Textures/CubeMapTexture.h"
#include "../Engine/Configuration.h"
#include "Textures/HeightMap.h"
#include "Logger/Log.h"
#include <FreeImage.h>
#include <algorithm>
#include <fstream>

CTextureLoader::CTextureLoader(GameEngine::IGraphicsApiPtr graphicsApi, std::vector<std::unique_ptr<CTexture>>& textures_vector, COpenGLLoader & openGLLoader)
    : graphicsApi_(graphicsApi)
	, textures(textures_vector)
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
	Log("File convert bgr2rgb" + file_location + ".");
	//image.data.resize(4*w*h);
	auto data = new uint8[4 * w*h];
	//bgr2rgb
	for (int j = 0; j<w*h; j++)
	{
        data[j * 4 + 0] = pixeles[j * 4 + 2];
        data[j * 4 + 1] = pixeles[j * 4 + 1];
        data[j * 4 + 2] = pixeles[j * 4 + 0];
        data[j * 4 + 3] = pixeles[j * 4 + 3];
	}

	image.data = std::vector<uint8>(data, data + 4*w*h);

	FreeImage_Unload(imagen);
	FreeImage_Unload(imagen2);
    Log("File: " + file_location + " is loaded.");
}

CTexture* CTextureLoader::LoadTexture(const std::string & file, bool applySizeLimit, bool opengl_pass, ObjectTextureType type, TextureFlip::Type flip_mode)
{
    for (auto& t : textures)
        if (t->GetFileName() == file)
            return t.get();

	SImagePtr texture(new SImage);
	ReadFile(EngineConf_GetFullDataPathAddToRequierd(file), *texture, applySizeLimit, flip_mode);

	switch (type)
	{
	case ObjectTextureType::MATERIAL:
         textures.emplace_back(new CMaterialTexture(graphicsApi_, false, file, file, texture));
		break;
	}
	if(opengl_pass)
        openGLLoader.AddObjectToOpenGLLoadingPass(textures.back().get());
    return textures.back().get();
}

CTexture * CTextureLoader::LoadTextureImmediately(const std::string & file, bool applySizeLimit, ObjectTextureType type, TextureFlip::Type flip_mode)
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

	std::vector<SImagePtr> images;
	images.resize(6);

	int x = 0;
	for (const auto& file : files)
	{
		images[x] = std::make_shared<SImage>();
        ReadFile(EngineConf_GetFullDataPathAddToRequierd(file), *images[x++], applySizeLimit, TextureFlip::Type::VERTICAL);
	}

    textures.emplace_back(new CCubeMapTexture(graphicsApi_, files[0], images));

    auto texture = textures.back().get();

	if(opengl_pass)
        openGLLoader.AddObjectToOpenGLLoadingPass(texture);

	return texture;
}

CTexture* CTextureLoader::LoadHeightMap(const std::string& filename, bool opengl_pass)
{	
	auto fp = fopen(filename.c_str(), "rb");

	if (!fp)
	{
		Log("[Error] GetFileType: wrong image format or file does not exist : " + filename);
		return nullptr;
	}

	struct Header
	{
		uint32 width;
		uint32 height;
	};

	Header header;
	fread(&header, sizeof(Header), 1, fp);

	Log(" Size : " + std::to_string(header.width) + "x" + std::to_string(header.height));

	SImagePtr texture(new SImage);
	auto& text = *texture;
	text.width = header.width;
	text.height = header.height;

	auto size = header.width * header.height;

	text.floatData.resize(size);
	//memset(&text.floatData[0], 0, size);
	fread(&text.floatData[0], sizeof(float), size, fp);
	fclose(fp);

	auto heightmap_texture = new HeightMap(graphicsApi_, true, filename, filename, texture);
	textures.emplace_back(heightmap_texture);

	if (opengl_pass)
		openGLLoader.AddObjectToOpenGLLoadingPass(heightmap_texture);

	return heightmap_texture;
}

GameEngine::IGraphicsApiPtr CTextureLoader::GetGraphicsApi()
{
	return graphicsApi_;
}
