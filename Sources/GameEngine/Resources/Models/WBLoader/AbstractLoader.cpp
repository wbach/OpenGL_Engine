#include "AbstractLoader.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "GameEngine/Engine/Configuration.h"
#include "Binary/BinaryWriter.h"
#include "Binary/BinaryReader.h"
#include "Utils/Utils.h"
#include <fstream>
#include "Logger/Log.h"

namespace WBLoader
{
	AbstractLoader::AbstractLoader(GameEngine::IGraphicsApiPtr graphicsApi, CTextureLoader& textureLoader)
		: textureLoader_(textureLoader)
		, graphicsApi_(graphicsApi)
		, loadedFromBin_(false)
	{
	}
	void AbstractLoader::Parse(const std::string& filename)
	{
		Utils::GetFileAndPath(filename, fileName_, filePath_);

		auto binFile = CreateBinPath(fileName_);
		if (EngineConf.useBinaryLoading && Utils::CheckFileExist(binFile))
		{
			ReadBinFile(binFile, textureLoader_);
			loadedFromBin_ = true;
			Log("Load from bin file :" + filename);
		}
		else
		{
			ParseFile(filename);
		}
	}
	std::unique_ptr<CModel> AbstractLoader::Create()
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		std::unique_ptr<CModel> newModel;

		if (loadedFromBin_)
		{
			newModel = CreateModelFromBin();
		}
		else
		{
			newModel = CreateModel();
			CreateBinFile(newModel, fileName_);
		}

		auto endTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		Log("Time : " + std::to_string(duration) + "ms.");
		return newModel;
	}
	std::unique_ptr<CModel> AbstractLoader::CreateModel()
	{
		float maxFactor = FindMaxFactor();

		auto newModel = std::make_unique<CModel>(maxFactor);

		for (auto& obj : objects)
		{
			NormalizeMatrix(obj.transformMatrix, maxFactor);

			for (auto& mesh : obj.meshes)
			{
				CMesh newMesh(graphicsApi_, mesh.material, obj.transformMatrix);
				IndexinVBO(mesh.vertexBuffer, newMesh.GetMeshDataRef());
				newModel->animationClips_ = mesh.animationClips_;
				newModel->skeleton_ = mesh.skeleton_;
				GameEngine::Animation::CalcInverseBindTransform(newModel->skeleton_, mat4(1.f));
				newModel->AddMesh(newMesh);
			}
		}
		objects.clear();
		return newModel;
	}

	std::unique_ptr<CModel> AbstractLoader::CreateModelFromBin()
	{
		return ReadBinFile(fileName_, textureLoader_);
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
