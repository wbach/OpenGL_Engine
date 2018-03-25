#include "HeightMap.h"
#include "Logger/Log.h"

using namespace GameEngine;

HeightMap::HeightMap(GameEngine::IGraphicsApiPtr graphicsApi, bool keepData, const std::string & file, const std::string & filepath, SImagePtr image)
	: CTexture(graphicsApi, file, filepath)
	, image(std::move(image))
	, keepData(keepData)
{
}

void HeightMap::OpenGLLoadingPass()
{
	if (image->floatData.empty() || isInit)
	{
		Log("[Error] OGL There was an error loading the texture : " + filename + ". floatData is null or is initialized.");
		return;
	}

	Log("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);
	id = graphicsApi_->CreateTexture(TextureType::FLOAT_TEXTURE_1C, TextureFilter::LINEAR, TextureMipmap::NONE, BufferAtachment::NONE, vec2ui(image->width, image->height), &image->floatData[0]);

	if (id == 0)
	{
		image->data.clear();
		Log("[Error] OGL There was an error loading the texture : " + filename + " cannot create texture.");
		return;
	}

	if (!keepData)
	{
		image->floatData.clear();
	}
	isInit = true;
	Log("File " + filename + " is in GPU. OpenGL pass succes");
}

SImagePtr HeightMap::GetImage()
{
	return image;
}
