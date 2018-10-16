#include "PhyscisTestScene.h"
#include "GameEngine/Engine/Engine.h"
#include "Scenes/PhysicsSceneFactory.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace PhysicsTestGame
{
	void Start(GameEngine::IGraphicsApiPtr gptr)
	{
		CEngine engine(gptr, makeBulletAdapter(), std::make_shared<PhysicsSceneFactory>());
		engine.Init();
		engine.sceneManager_.SetActiveScene("PhysicsScene");
		engine.GameLoop();
	}
}// PhysicsTestGame
