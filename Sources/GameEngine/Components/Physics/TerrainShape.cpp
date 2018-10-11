#include "TerrainShape.h"

#include "GameEngine/Objects/RenderAble/Terrain/TerrainDef.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"

#include <algorithm>

namespace GameEngine
{
	namespace Components
	{
		ComponentsType TerrainShape::type = ComponentsType::TerrainShape;

		TerrainShape::TerrainShape()
			: CollisionShape(ComponentsType::TerrainShape)
			, size_(1)
			, heightFactor_(1)
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

			auto minElementIter = std::min_element(data_->begin(), data_->end());
			auto maxElementIter = std::max_element(data_->begin(), data_->end());

			auto minElement = minElementIter != data_->end() ? *minElementIter : 0.f;
			auto maxElement = maxElementIter != data_->end() ? *maxElementIter : 0.f;

			auto distance = fabs(maxElement - minElement);
			auto h = maxElement - (distance / 2.f);
			thisObject->worldTransform.IncrasePosition(vec3(0, h, 0));
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

