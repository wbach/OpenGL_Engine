#pragma once
#include "Types.h"
#include "Common/Controllers/IController.h"
namespace GameEngine
{
	struct BaseCollider : public common::Controllers::IController
	{
		BaseCollider() 
			: common::Controllers::IController(common::Controllers::Types::Collider)
		{}
		virtual ~BaseCollider() {}
		bool isTrigger_;
		vec3 center_;
	};
}