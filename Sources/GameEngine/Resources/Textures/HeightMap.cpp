#include "HeightMap.h"
#include "Logger/Log.h"

HeightMap::HeightMap(bool keepData, const std::string & file, const std::string & filepath, SImagePtr image)
	: CTexture(file, filepath)
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

	//for(auto& d : image.shortData)


	glGenTextures(1, &id);
	GLenum hubo_error = glGetError();

	if (hubo_error)
	{
		image->data.clear();
		Log("[Error] OGL There was an error loading the texture : " + filename + " cannot create texture.");
		return;
	}

	// "Bind" the newly created texture : all future texture functions will modify this texture
	Log("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);

	glBindTexture(GL_TEXTURE_2D, id);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, image->width, image->height, 0, GL_RED, GL_FLOAT, (GLvoid*) &image->floatData[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

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
