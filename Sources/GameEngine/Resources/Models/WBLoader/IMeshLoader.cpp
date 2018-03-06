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
				mesh.Normalize(maxFactor);
				mesh.IndexinVBO();
				output.emplace_back(mesh.material, mesh.fpostions, mesh.fuvs, mesh.fnormal, mesh.ftangents, mesh.indices, mesh.jointIds, mesh.bonesWeights, obj.transformMatrix);
			}
		}

		return output;
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