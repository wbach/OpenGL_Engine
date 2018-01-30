#pragma once
#include "Scene/ISceneFactory.h"

namespace Network
{
	class CGateway;
} // Network

namespace MmmoRpg
{
	class SceneFactory : public GameEngine::ISceneFactory
	{
	public:
		SceneFactory(Network::CGateway& gateway, std::string& serverAdress)
			: gateway_(gateway)
			, serverAdress_(serverAdress)
		{

		}
		// Inherited via ISceneFactory
		virtual std::unique_ptr<CScene> Create(const std::string &) override;

	private:
		Network::CGateway& gateway_;
		std::string& serverAdress_;
	};
}