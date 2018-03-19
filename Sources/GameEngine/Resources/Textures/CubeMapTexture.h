#pragma once
#include "Texture.h"
#include "Image.h"
#include <stdexcept>
#include <vector>

class CCubeMapTexture : public CTexture
{
public:
	CCubeMapTexture(GameEngine::IGraphicsApiPtr graphicsApi, const std::string& name, const std::vector<SImagePtr>&  image);
	virtual void OpenGLLoadingPass() override;

private:
	SImagePtr images[6];
    bool keepData = false;
};
