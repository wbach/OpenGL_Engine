#include "ScenesFactory.h"
#include "Login.h"
#include "SelectCharacter/SelectCharacterScene.h"
#include "../../MainScene.h"
#include "Logger/Log.h"

namespace MmmoRpg
{
	std::unique_ptr<CScene> SceneFactory::Create(const std::string& sceneName)
	{
		if (sceneName == "MainScene")
			return std::make_unique<MainScene>();
		if (sceneName == "LoginScene")
			return std::make_unique<LoginScene>(gateway_, serverAdress_);
		if (sceneName == "SelectCharacterScene")
			return std::make_unique<SelectCharacterScene>(gateway_);

		Log("SceneFactory::Create scene : " + sceneName + " not found.");

		return nullptr;
	}
} // MmmoRpg