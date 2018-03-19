#pragma once
#include "Texture.h"
#include "Image.h"
#include <stdexcept>

class CMaterialTexture : public CTexture
{
public:
	CMaterialTexture(GameEngine::IGraphicsApiPtr graphicsApi, bool keepData, const std::string& file , const std::string& filepath, SImagePtr image );
	virtual void OpenGLLoadingPass() override;

private:
	SImagePtr image;
    bool keepData = false;
};
