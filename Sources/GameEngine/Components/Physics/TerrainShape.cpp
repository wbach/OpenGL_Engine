#include "TerrainShape.h"

#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType TerrainShape::type = ComponentsType::TerrainShape;

		TerrainShape::TerrainShape()
			: CollisionShape(ComponentsType::TerrainShape)
			, size_(1)
		{
		}
		void TerrainShape::Update()
		{
		}
		void TerrainShape::ReqisterFunctions()
		{
			RegisterFunction(FunctionType::Awake, std::bind(&TerrainShape::OnAwake, this));
		}
		void TerrainShape::OnAwake()
		{
			collisionShapeId_ = physicsApi_->CreateTerrainColider(positionOffset_, size_, *data_, heightFactor_);
		}
		void TerrainShape::SetSize(const vec2ui & size)
		{
			size_ = size;
		}
		void TerrainShape::SetHeightFactor(float factor)
		{
			heightFactor_ = factor;
		}
		void TerrainShape::SetData(std::vector<float>* data)
		{
			data_ = data;
		}
	} // Components
} // GameEngine

