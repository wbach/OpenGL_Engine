#pragma once
#include "Texture.h"
#include "Image.h"
#include <stdexcept>
#include <vector>

class CCubeMapTexture : public CTexture
{
public:
	CCubeMapTexture(const std::string& name, const std::vector<SImage>& image);
	virtual void OpenGLLoadingPass() override;

private:
    SImage images[6];
    bool keepData = false;
};
