#pragma once
#include "Resources/TextureLoader.h"
#include "OpenGLLoaderMock.h"

struct TextureLoaderMock : public CTextureLoader
{
	TextureLoaderMock() : CTextureLoader(textures, openGlmock)
	{
	}

	OpenGLLoaderMock openGlmock;
	std::vector<std::unique_ptr<CTexture>> textures;
};