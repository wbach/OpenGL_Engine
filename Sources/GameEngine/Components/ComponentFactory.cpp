#include "ComponentFactory.h"
#include "AbstractComponent.h"
#include "Animation/Animator.h"
#include "Renderer/RendererComponent.hpp"
#include "Renderer/TreeRendererComponent.h"
#include "Renderer/ParticleEffectComponent.h"
#include "Physics/BoxShape.h"
#include "Physics/SphereShape.h"
#include "Physics/TerrainShape.h"
#include "Physics/MeshShape.h"
#include "Physics/Rigidbody.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentFactory::ComponentFactory(ComponentController& componentController, Time& time, std::shared_ptr<CResourceManager>& resourceManager, std::shared_ptr<ICamera>& camera, std::shared_ptr<Physics::IPhysicsApi>* physicsApi)
			: componentController_(componentController)
			, resourceManager_(resourceManager)
			, physicsApi_(physicsApi)
			, camera_(camera)
			, time_(time)
		{
		}
		std::unique_ptr<AbstractComponent> ComponentFactory::Create(ComponentsType type, CGameObject* ptr)
		{
			switch (type)
			{
			case ComponentsType::Animator:
			{
				return CreateAndBasicInitialize<Animator>(ptr);
			}
			case ComponentsType::Renderer:
			{
				return CreateAndBasicInitialize<RendererComponent>(ptr);
			}
			case ComponentsType::TreeRenderer:
			{
				return CreateAndBasicInitialize<TreeRendererComponent>(ptr);
			}
			case ComponentsType::ParticleEffect:
			{
				return CreateAndBasicInitialize<ParticleEffectComponent>(ptr);
			}
			case ComponentsType::BoxShape:
			{
				return CreateAndBasicInitialize<BoxShape>(ptr);
			}
			case ComponentsType::SphereShape:
			{
				return CreateAndBasicInitialize<SphereShape>(ptr);
			}
			case ComponentsType::TerrainShape:
			{
				return CreateAndBasicInitialize<TerrainShape>(ptr);
			}
			case ComponentsType::MeshShape:
			{
				return CreateAndBasicInitialize<MeshShape>(ptr);
			}
			case ComponentsType::Rigidbody:
			{
				return CreateAndBasicInitialize<Rigidbody>(ptr);
			}
			case ComponentsType::BoxCollider:
			{
				break;
			}
			case ComponentsType::CollisionShape:
			{
				break;
			}

			}
			return nullptr;
		}
		void ComponentFactory::SetRendererManager(Renderer::RenderersManager * rendererManager)
		{
			rendererManager_ = rendererManager;
		}
		template<class T>
		std::unique_ptr<T> ComponentFactory::CreateAndBasicInitialize(CGameObject* ptr)
		{
			auto comp = std::make_unique<T>();
			comp->SetComponentController(&componentController_);
			comp->SetTime(&time_);
			comp->SetCamera(&camera_);
			comp->SetResourceManager(resourceManager_.get());
			comp->SetRendererManager(rendererManager_);
			comp->SetPhysicsApi(physicsApi_->get());
			comp->SetObjectPtr(ptr);
			return comp;
		}
	} // Components
} // GameEngine
