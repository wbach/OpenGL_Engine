#pragma once
#include "Resources/TextureLoader.h"
#include "../Api/GraphicsApiMock.h"
#include "OpenGLLoaderMock.h"

struct TextureLoaderMock : public CTextureLoader
{
	TextureLoaderMock()
		: CTextureLoader(std::make_shared<GameEngine::GraphicsApiMock>(), textures, openGlmock)
	{
	}

	GameEngine::IGraphicsApiPtr graphicsMock_;
	OpenGLLoaderMock openGlmock;
	std::vector<std::unique_ptr<CTexture>> textures;
};