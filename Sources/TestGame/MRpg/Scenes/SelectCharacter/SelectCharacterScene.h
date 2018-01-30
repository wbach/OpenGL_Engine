#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../UtilsNetwork/Messages/GetCharacters/CharacterInfo.h"
#include <vector>

namespace Network
{
	class CGateway;
}

namespace MmmoRpg
{
	class SelectCharacterScene : public CScene
	{
	public:
		SelectCharacterScene(Network::CGateway& gateway);
		virtual ~SelectCharacterScene() override;
		virtual int		Initialize() override;
		virtual void	PostInitialize() override;
		virtual int		Update(float deltaTime) override;

	private:
		Network::CGateway& gateway_;
		std::vector<Network::CharacterInfo> charactersData_;
	};
}