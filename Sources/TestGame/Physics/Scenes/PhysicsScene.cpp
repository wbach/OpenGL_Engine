#include "PhysicsScene.h"
#include "SingleTon.h"
#include "GameEngine/Engine/AplicationContext.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/RenderAble/Flora/Grass/Grass.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/GuiText.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "GameEngine/Objects/RenderAble/Terrain/Terrain.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainWrapper.h"
#include "GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Renderer/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/TreeRendererComponent.h"
#include "GameEngine/Components/Renderer/ParticleEffectComponent.h"
#include "GameEngine/Api/BlendFunctionsTypes.h"
#include "GLM/GLMUtils.h"
#include "Thread.hpp"



#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"


#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

struct Bt
{
	std::unique_ptr<btDynamicsWorld> btDynamicWorld;
	std::unique_ptr<btBroadphaseInterface> btBroadPhase;
	std::unique_ptr<btConstraintSolver> btSolver;
	std::unique_ptr<btCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btDispatcher> btDispacher;

	void CreateTerrain(btCollisionShape* shape)
	{
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(160, -10, 160));
		
		btScalar mass(0.);
		ground_ = createRigidBody(mass, groundTransform, shape, btVector4(0, 0, 1, 1));

		collisionShapes_.push_back(shape);
	}

	Bt()
	{
		collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
		btDispacher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
		btBroadPhase = std::make_unique<btDbvtBroadphase>();
		btSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
		btDynamicWorld = std::make_unique<btDiscreteDynamicsWorld>(btDispacher.get(), btBroadPhase.get(), btSolver.get(), collisionConfiguration.get());
		btDynamicWorld->setGravity(btVector3(0, -10, 0));


		//btBoxShape* groundShape = createBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		//CreateTerrain(groundShape);


		{
			//create a few dynamic rigidbodies
			// Re-using the same collision is better for memory usage and performance

			btBoxShape* colShape = createBoxShape(btVector3(.1, .1, .1));


			//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
			collisionShapes_.push_back(colShape);

			/// Create Dynamic Objects
			btTransform startTransform;
			startTransform.setIdentity();

			btScalar	mass(1.f);

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShape->calculateLocalInertia(mass, localInertia);

			const float scale = 0.2;
			for (int k = 0; k < ARRAY_SIZE_Y; k++)
			{
				for (int i = 0; i < ARRAY_SIZE_X; i++)
				{
					for (int j = 0; j < ARRAY_SIZE_Z; j++)
					{
						startTransform.setOrigin(btVector3(
							btScalar(scale*i),
							btScalar(2 + scale * k + 100),
							btScalar(scale*j)));

						auto body = createRigidBody(mass, startTransform, colShape);
						boxes_.push_back(body);
					}
				}
			}
		}
	}

	btRigidBody* CreateBox(const vec3& pos, float scale)
	{
			//create a few dynamic rigidbodies
			// Re-using the same collision is better for memory usage and performance

			btBoxShape* colShape = createBoxShape(btVector3(scale /2.f, scale / 2.f, scale / 2.f));


			//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
			collisionShapes_.push_back(colShape);

			/// Create Dynamic Objects
			btTransform startTransform;
			startTransform.setIdentity();

			btScalar	mass(1.f);

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShape->calculateLocalInertia(mass, localInertia);

			startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

			auto body = createRigidBody(mass, startTransform, colShape);
			boxes_.push_back(body);

			return body;
	}

	btBoxShape* createBoxShape(const btVector3& halfExtents)
	{
		btBoxShape* box = new btBoxShape(halfExtents);
		return box;
	}

	btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color = btVector4(1, 0, 0, 1))
	{
		btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

		btRigidBody* body = new btRigidBody(cInfo);

		body->setUserIndex(-1);
		btDynamicWorld->addRigidBody(body);
		return body;
	}
	void deleteRigidBody(btRigidBody* body)
	{
		int graphicsUid = body->getUserIndex();

		btDynamicWorld->removeRigidBody(body);
		btMotionState* ms = body->getMotionState();
		delete body;
		delete ms;

	}
	btAlignedObjectArray<btCollisionShape*>	collisionShapes_;
	std::list<btRigidBody*> boxes_;
	btRigidBody* ground_;
	std::unique_ptr<btHeightfieldTerrainShape> terrainShape;

	~Bt()
	{
		for (auto& r : boxes_)
			delete r;

		delete ground_;
	}
};


Bt bt;


vec3 Convert(const btVector3& v)
{
	return vec3(v.getX(), v.getY(), v.getZ());
}

btVector3 Convert(const vec3& v)
{
	return btVector3(v.x, v.y, v.z);
}

namespace PhysicsTestGame
{

	TerrainTexturesMap CreateTerrainTexturesMap()
	{
		return
		{
			{ Terrain::blendMap , "Textures/Terrain/BlendMaps/testBlendMap.png"},
			{ Terrain::backgorundTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Grass_01_Diffuse_01.png" },
			{ Terrain::redTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Path_03_Diffuse_01.png",  },
			{ Terrain::rockTexture, "Textures/Terrain/Ground/G3_Nature_Wall_Stone_12_Diffuse_01.png", },
			{ Terrain::snowTexture, "Textures/Terrain/Ground/snow512.png", },
			{ Terrain::greenTexture,"Textures/Terrain/Ground/grassFlowers.png"},
			{ Terrain::blueTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Forest_01_Diffuse_01.png" },
			{ Terrain::displacementMap, "Textures/Terrain/HeightMaps/Terrain.terrain" }
		};
	}

	PhysicsScene::PhysicsScene()
		: GameEngine::Scene("PhysicsScene")
	{
		const unsigned int dataSize = 4096;
		data = new float[dataSize * dataSize];
		for (unsigned int x = 0; x < dataSize * dataSize; x++)
			data[x] = 0.f;
	}

	PhysicsScene::~PhysicsScene()
	{
		delete[] data;
		Log(__FUNCTION__);
	}

	float Random()
	{
		return static_cast<float>(rand() % 100000) / 100000.f;
	}

	void PhysicsScene::AddBox(const vec3& pos, const vec3& dir, float scale)
	{
		auto barrel = AddGameObjectInstance(scale, pos);

		btScalar p, y, r;
		auto box = bt.CreateBox(pos, scale);
		box->getWorldTransform().getRotation().getEulerZYX(y, p, r);
		box->setLinearVelocity(Convert(dir));
		barrel->worldTransform.SetRotation(vec3(y, p, r));
		barrel->worldTransform.TakeSnapShoot();

		AddComponent<GameEngine::Components::RendererComponent>(barrel)->AddModel("Meshes/Cube.obj");
		
	}

	int PhysicsScene::Initialize()
	{
		camera = std::make_unique<GameEngine::FirstPersonCamera>(inputManager_, displayManager_);
		camera->SetPosition(vec3(-10, 5, 0));

		for (auto& box : bt.boxes_)
		{
			auto pos = box->getWorldTransform().getOrigin();

			auto barrel = AddGameObjectInstance(1.f, Convert(pos));

			btScalar p, y, r;
			box->getWorldTransform().getRotation().getEulerZYX(y, p, r);
			barrel->worldTransform.SetRotation(vec3(y, p, r));
			barrel->worldTransform.SetScale(0.2);
			barrel->worldTransform.TakeSnapShoot();

			AddComponent<GameEngine::Components::RendererComponent>(barrel)->AddModel("Meshes/Cube.obj");
		}

		auto terrain_textures = CreateTerrainTexturesMap();
		auto terrain = AddTerrain(terrain_textures, glm::vec3(1));
		terrain->GetHeight(0, 0);
		//auto data = terrain->GetHeightData();

		const int MIN_HEIGHT = 0.f, MAX_HEIGHT = 100.f;

		const auto& tdata = terrain->GetHeightData();

		//unsigned int i = 0;
		//for (auto y = 0u; y < terrain->GetSize().y * terrain->GetSize().x;; y++)
		//{
		//	data[i];
		//}
		

		//	const void* heightfieldData, btScalar heightScale,btScalar minHeight, btScalar maxHeight,	int upAxis, PHY_ScalarType heightDataType,bool flipQuadEdges);
		bt.terrainShape = std::make_unique<btHeightfieldTerrainShape>(terrain->GetSize().x, terrain->GetSize().y, &tdata[0], 1.f, -100, 100.f, 1, PHY_FLOAT, false);
		bt.CreateTerrain(bt.terrainShape.get());


		inputManager_->SubscribeOnKeyDown(KeyCodes::F, [&]()
		{
			auto dir = GetCamera()->GetDirection();
			dir = glm::normalize(dir);
			auto pos = GetCamera()->GetPosition();
			AddBox(pos, dir * 100.f, 0.2f);
			Log("Dir : " + Utils::ToString(dir) + ", Pos : " + Utils::ToString(pos) + ", Objecsts : " + std::to_string(objects_.size()));
		});

		inputManager_->SubscribeOnKeyDown(KeyCodes::R, [&]()
		{
			simulatePhysics.store(!simulatePhysics.load());
		});

		return 0;
	}

	CTerrain* PhysicsScene::AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position)
	{
		auto terrain = ObjectBuilder::CreateTerrain(*resourceManager_, textures);
		if (terrain == nullptr)
		{
			Error("MainScene::AddTerrain : terrain is nullptr.");
			return nullptr;
		}
		AddGameObject(terrain, position);
		renderersManager_->Subscribe(terrain);
		terrains_.push_back(terrain);

		return static_cast<CTerrainWrapper*>(terrain)->Get();
	}

	CGameObject* PhysicsScene::AddGameObjectInstance(float scale, const vec3& position, bool isDynamic)
	{
		auto obj = new CGameObject();
		obj->worldTransform.SetScale(scale);
		obj->worldTransform.isDynamic_ = isDynamic;
		AddGameObject(obj, position);
		obj->worldTransform.TakeSnapShoot();
		objects_.push_back(obj);
		return obj;
	}

	int PhysicsScene::Update(float dt)
	{
		if (camera == nullptr)
		{
			Log("PhysicsScene::Update camera is nullptr.");
			return -1;
		}

		if (simulatePhysics)
			bt.btDynamicWorld->stepSimulation(1.0f / 60.f);

		uint32 x = 0;
		for (auto& box : bt.boxes_)
		{
			auto pos = box->getWorldTransform().getOrigin();

			vec3 rot;
			box->getWorldTransform().getRotation().getEulerZYX(rot.z, rot.y, rot.x);
			rot = vec3(Utils::ToDegrees(rot.x), Utils::ToDegrees(rot.y), Utils::ToDegrees(rot.z));
			objects_[x]->worldTransform.SetRotation(rot);

			objects_[x]->worldTransform.SetPosition(Convert(pos));
			objects_[x]->worldTransform.TakeSnapShoot();
			++x;
			vec3(pos.getX(), pos.getY(), pos.getZ());
		}


		//auto dir = GetCamera()->GetDirection();
		//dir = glm::normalize(dir);
		//auto pos = GetCamera()->GetPosition();
		//AddBox(pos, dir * -100.f, 2.2f);

		return 0;
	}
} // PhysicsTestGame
