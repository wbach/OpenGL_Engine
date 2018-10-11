#include "MeshShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Models/Mesh.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType MeshShape::type = ComponentsType::MeshShape;

		MeshShape::MeshShape()
			: CollisionShape(ComponentsType::MeshShape)
			, size_(1.f)
			, model_(nullptr)
		{
		}
		void MeshShape::Update()
		{
		}
		void MeshShape::ReqisterFunctions()
		{
			RegisterFunction(FunctionType::Awake, std::bind(&MeshShape::OnAwake, this));
		}
		void MeshShape::OnAwake()
		{
			if (model_ == nullptr)
			{
				return;
			}

			const auto& meshes = model_->GetMeshes();

			for (const auto& mesh : meshes)
			{
				auto data = mesh.GetCMeshDataRef();
				collisionShapeId_ = physicsApi_->CreateMeshCollider(positionOffset_, data.positions_, data.indices_);
			}
		}
		void MeshShape::SetModel(CModel * model)
		{
			model_ = model;
		}
	} // Components
} // GameEngine
