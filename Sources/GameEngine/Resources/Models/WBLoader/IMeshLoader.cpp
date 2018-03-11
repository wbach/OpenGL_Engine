#include "IMeshLoader.h"

namespace WBLoader
{
	std::list<CMesh> AbstractMeshLoader::CreateFinalMesh()
	{
		std::list<CMesh> output;

		float maxFactor = FindMaxFactor();

		for (auto& obj : objects)
		{
			for (auto& mesh : obj.meshes)
			{
				mesh.IndexinVBO();
				NormalizeMatrix(obj.transformMatrix, maxFactor);
				output.emplace_back(mesh.material, mesh.fpostions, mesh.fuvs, mesh.fnormal, mesh.ftangents, mesh.indices, mesh.jointIds, mesh.bonesWeights, obj.transformMatrix);
				output.back().animator_ = std::move(mesh.animator_);
				output.back().rootJoint_ = std::move(mesh.rootJoint_);
				output.back().rootJoint_.calcInverseBindTransform(mat4(1.f));
			}
		}

		return output;
	}
	void AbstractMeshLoader::NormalizeMatrix(mat4 & mat, float factor) const
	{
		mat *= glm::scale(vec3(1.f / factor));
	}
	float AbstractMeshLoader::FindMaxFactor() const
	{
		float maxFactor = -std::numeric_limits<float>::max();

		for (auto& obj : objects)
		{
			for (auto& mesh : obj.meshes)
			{
				float f = mesh.GetScaleFactor();
				if (f > maxFactor)
					maxFactor = f;
			}
		}
		return maxFactor;
	}
} // WBLoader