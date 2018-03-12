#include "AbstractLoader.h"
#include "GameEngine/Animations/AnimationUtils.h"

namespace WBLoader
{
	std::unique_ptr<CModel> AbstractLoader::Create()
	{
		auto newModel = std::make_unique<CModel>();

		float maxFactor = FindMaxFactor();

		for (auto& obj : objects)
		{
			NormalizeMatrix(obj.transformMatrix, maxFactor);

			for (auto& mesh : obj.meshes)
			{
				mesh.IndexinVBO();
				CMesh newMesh(mesh.material, mesh.fpostions, mesh.fuvs, mesh.fnormal, mesh.ftangents, mesh.indices, mesh.jointIds, mesh.bonesWeights, obj.transformMatrix);
				newMesh.animator_ = std::move(mesh.animator_);
				newMesh.rootJoint_ = std::move(mesh.rootJoint_);
				GameEngine::Animation::CalcInverseBindTransform(newMesh.rootJoint_, mat4(1.f));
				newModel->AddMesh(newMesh);
			}
		}

		return newModel;
	}
	void AbstractLoader::NormalizeMatrix(mat4 & mat, float factor) const
	{
		mat *= glm::scale(vec3(1.f / factor));
	}
	float AbstractLoader::FindMaxFactor() const
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