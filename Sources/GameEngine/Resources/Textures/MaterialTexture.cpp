#include "MaterialTexture.h"
#include "Logger/Log.h"

using namespace GameEngine;

CMaterialTexture::CMaterialTexture(GameEngine::IGraphicsApiPtr graphicsApi, bool keepData, const std::string & file, const std::string & filepath, SImagePtr image)
	: CTexture(graphicsApi, file, filepath)
    , image(image)
    , keepData(keepData)
{
}

void CMaterialTexture::OpenGLLoadingPass()
{
    if (image->data.empty()|| isInit)
	{
		Log("[Error] OGL There was an error loading the texture : " + filename + ". data is null or is initialized.");
		return;
	}

	Log("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);
	id = graphicsApi_->CreateTexture(TextureType::U8_RGBA, TextureFilter::NEAREST, TextureMipmap::LINEAR, BufferAtachment::NONE, vec2ui(image->width, image->height), &image->data[0]);

	if (id == 0)
	{
		image->data.clear();
		Log("[Error] OGL There was an error loading the texture : " + filename + " cannot create texture.");
		return;
	}
    if (!keepData)
	{
        image->data.clear();
	}		
    isInit = true;
	Log("File " + filename + " is in GPU. OpenGL pass succes");
}
