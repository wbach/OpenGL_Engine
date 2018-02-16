#include "CubeMapTexture.h"
#include "Logger/Log.h"

CCubeMapTexture::CCubeMapTexture(const std::string& name, const std::vector<SImagePtr>& image)
	: CTexture(name, name)
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
    glGenTextures(1, &id);
	GLenum hubo_error = glGetError();

	if (hubo_error)
	{
		for (auto& i : images)
			i->data.clear();

		Log("[Error] OGL There was an error loading the texture : " + filename);
		return;
	}
	Log("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (int x = 0; x < 6; x++)
	{
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, images[x]->width, images[x]->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) &images[x]->data[0]);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		images[x]->data.clear();
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    isInit = true;
}
