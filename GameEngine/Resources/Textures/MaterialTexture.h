#pragma once
#include "../../Debug_/Log.h"
#include "Texture.h"
#include "Image.h"
#include <stdexcept>

class CMaterialTexture : public CTexture
{
public:
	CMaterialTexture(bool keepData, const std::string& file , const std::string& filepath, const SImage& image );
	virtual void OpenGLLoadingPass() override;
private:
	SImage m_Image;

	bool m_KeepData = false;
};
