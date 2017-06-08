#include "MaterialTexture.h"


CMaterialTexture::CMaterialTexture(bool keepData, const std::string & file, const std::string & filepath, const SImage & image)
	: CTexture(file, filepath)
    , image(image)
    , keepData(keepData)
{
}


void CMaterialTexture::OpenGLLoadingPass()
{
    if (image.data == nullptr || isInit)
	{
		Log("[Error] OGL There was an error loading the texture : " + filename);
		return;
	}

    glGenTextures(1, &id);
	GLenum hubo_error = glGetError();

	if (hubo_error)
	{
        delete[] image.data;
		Log("[Error] OGL There was an error loading the texture : " + filename);
		return;
	}

	// "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, id);
	// Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (!keepData)
	{
        delete[] image.data;
        image.data = nullptr;
	}		
    isInit = true;

	Log("File " + filename + " is in GPU. OpenGL pass succes");
}
