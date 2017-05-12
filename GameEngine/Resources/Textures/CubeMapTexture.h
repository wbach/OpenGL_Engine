#pragma once
#include "../../Debug_/Log.h"
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
	SImage m_Images[6];

	bool m_KeepData = false;
};
