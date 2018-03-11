#pragma once
#include "Types.h"
#include <vector>

namespace GameEngine
{
	namespace Animation
	{
		struct Joint
		{
			uint32 id;
			mat4 transform;

			std::string name;
			std::vector<Joint> children;

			const Joint* GetJoint(uint32 fid) const
			{
				if (id == fid)
					return this;

				for (auto& j : children)
				{
					auto f = j.GetJoint(fid);
					if (f != nullptr)
						return f;
				}
				return nullptr;
			}

			mat4 animatedTransform;
			//mat4 localBindTransform;
			mat4 inverseBindTransform;


			void calcInverseBindTransform(const mat4& parentBindTransform)
			{
				mat4 bindTransform = parentBindTransform * transform;
				inverseBindTransform = glm::inverse(bindTransform);

				for (Joint& child : children)
				{
					child.calcInverseBindTransform(bindTransform);
				}
			}

		};
	} // Animation
} // GameEngine
