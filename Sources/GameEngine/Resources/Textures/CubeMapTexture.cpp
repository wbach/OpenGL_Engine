#include "CubeMapTexture.h"
#include "Logger/Log.h"

CCubeMapTexture::CCubeMapTexture(GameEngine::IGraphicsApiPtr graphicsApi, const std::string& name, const std::vector<SImagePtr>& image)
	: CTexture(graphicsApi, name, name)
{
	if (image.size() != 6)
	{
		Log("Cube map texture need 6 texutres : " + name);
	}
	else
	{
		for (int x = 0; x < 6; x++)
            images[x] = image[x];
	}	
}

void CCubeMapTexture::OpenGLLoadingPass()
{
	if (isInit)
	{
		Log("[Error] OGL There was an error loading the texture : " + filename + ". data is null or is initialized.");
		return;
	}

	std::vector<void*> data;
	data.resize(6);

	for (int x = 0; x < 6; x++)
	{
		data[x] = &images[x]->data[0];
	}

	id = graphicsApi_->CreateCubMapTexture( vec2ui(images[0]->width, images[0]->height), data);

	for (auto& i : images)
		i->data.clear();

	if (id == 0)
	{
		Log("[Error] OGL There was an error loading the texture : " + filename);
		return;
	}

    isInit = true;
}
