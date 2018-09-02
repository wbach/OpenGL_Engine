#include "PhyscisTestScene.h"
#include "GameEngine/Engine/Engine.h"
#include "Scenes/PhysicsSceneFactory.h"

namespace PhysicsTestGame
{
	void Start(GameEngine::IGraphicsApiPtr gptr)
	{
		GameEngine::CEngine engine(gptr, std::make_shared<PhysicsSceneFactory>());
		engine.Init();
		engine.sceneManager_.SetActiveScene("PhysicsScene");
		engine.GameLoop();
	}
}// PhysicsTestGame
