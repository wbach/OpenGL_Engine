#pragma once
#include "Texture.h"
#include "Image.h"
#include <stdexcept>

class HeightMap : public CTexture
{
public:
	HeightMap(bool keepData, const std::string& file, const std::string& filepath, SImagePtr image);
	virtual void OpenGLLoadingPass() override;
	SImagePtr GetImage();

private:
	SImagePtr image;
	bool keepData = false;
};
