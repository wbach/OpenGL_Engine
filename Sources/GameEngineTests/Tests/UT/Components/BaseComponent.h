#pragma once
#include <gtest/gtest.h>
#include "GameEngine/Time/Time.h"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

struct BaseComponentTestSchould : public ::testing::Test
{
	BaseComponentTestSchould();
	void Init(AbstractComponent* component);

	std::shared_ptr<GraphicsApiMock> graphicsApiMock_;
	Time time_;
	std::shared_ptr<CCamera> camera_;
	CGameObject obj_;
	CResourceManager resourcesManager_;
	ComponentController componentController_;
	Renderer::RenderersManager renderersManager_;
};
