#include "AbstractLoader.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "GameEngine/Engine/Configuration.h"
#include "Utils/Utils.h"
#include <fstream>
#include "Logger/Log.h"

namespace WBLoader
{
	const std::string SEPARATE_STRING = "**************************";

	std::string CreateBinPath(const std::string& filename)
	{
		return EngineConf.dataFilesLocation + "/Binary/" + filename + ".bin";
	}
	void WriteToFile(std::fstream& file, const std::string& str)
	{
		uint32 size = str.size();
		file.write(reinterpret_cast<const char*>(&size), sizeof(uint32));
		if (!str.empty())
			file.write(str.c_str(), sizeof(char) * str.size());
	}
	void WriteToFile(std::fstream& file, CTexture* texture)
	{
		if (texture == nullptr)
		{
			WriteToFile(file, std::string());
		}
		else
		{
			WriteToFile(file, texture->GetFilPath());
		}
	}
	template<class T>
	void WriteToFile(std::fstream& file, const std::vector<T>& v)
	{
		uint32 size = v.size();
		file.write(reinterpret_cast<const char*>(&size), sizeof(uint32));

		if (!v.empty())
			file.write(reinterpret_cast<const char*>(&v[0]), sizeof(T) * v.size());
	}

	template<class T>
	void WriteToFile(std::fstream& file, T value)
	{
		file.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	void WriteToFile(std::fstream& file, const GameEngine::Animation::JointTransform& jointTransform)
	{
		WriteToFile(file, jointTransform.position);
		WriteToFile(file, jointTransform.rotation);
	}
	void WriteToFile(std::fstream& file, const GameEngine::Animation::KeyFrame& keyFrame)
	{
		WriteToFile(file, keyFrame.timeStamp);
		WriteToFile(file, keyFrame.transforms.size());
		for (const auto& jointTransform : keyFrame.transforms)
		{
			WriteToFile(file, jointTransform.first);
			WriteToFile(file, jointTransform.second);
		}
		WriteToFile(file, keyFrame.idTransforms_.size());
		for (const auto& jointTransform : keyFrame.idTransforms_)
		{
			WriteToFile(file, jointTransform.first);
			WriteToFile(file, jointTransform.second);
		}
	}
	void WriteToFile(std::fstream& file, const GameEngine::Animation::AnimationClip& clip)
	{
		WriteToFile(file, clip.name);
		WriteToFile(file, clip.GetLength());
		const auto& frames = clip.GetFrames();
		WriteToFile(file, frames.size());
		for (const auto& keyFrame : frames)
		{
			WriteToFile(file, keyFrame);
		}
	}

	void WriteToFile(std::fstream& file, const GameEngine::Animation::Joint& joint)
	{
		WriteToFile(file, joint.id);
		WriteToFile(file, joint.size);
		WriteToFile(file, joint.name);
		WriteToFile(file, joint.transform);
		WriteToFile(file, joint.invtransform);
		WriteToFile(file, joint.animatedTransform);
		WriteToFile(file, joint.children.size());
		for (const auto& child : joint.children)
		{
			WriteToFile(file, child);
		}
	}
	void WriteToFile(std::fstream& file, const AnimationClipsMap& animations)
	{
		WriteToFile(file, animations.size());
		for (const auto& clip : animations)
		{
			WriteToFile(file, clip.first);
			WriteToFile(file, clip.second);
		}
	}
	void WriteToFile(std::fstream& file, const GameEngine::MeshRawData& data,
		const SMaterial& material, const mat4& objTransform)
	{
		WriteToFile(file, material.name);
		WriteToFile(file, material.ambient);
		WriteToFile(file, material.diffuse);
		WriteToFile(file, material.specular);
		WriteToFile(file, material.shineDamper);
		WriteToFile(file, material.indexOfRefraction);
		WriteToFile(file, material.reflectivity);
		WriteToFile(file, material.isTransparency);
		WriteToFile(file, material.useFakeLighting);
		WriteToFile(file, material.diffuseTexture);
		WriteToFile(file, material.normalTexture);
		WriteToFile(file, material.ambientTexture);
		WriteToFile(file, material.specularTexture);
		WriteToFile(file, data.bitangents_);
		WriteToFile(file, data.bonesWeights_);
		WriteToFile(file, data.indices_);
		WriteToFile(file, data.instancedMatrixes_);
		WriteToFile(file, data.joinIds_);
		WriteToFile(file, data.normals_);
		WriteToFile(file, data.positions_);
		WriteToFile(file, data.tangents_);
		WriteToFile(file, data.textCoords_);
		WriteToFile(file, objTransform);
	}
	template<class T>
	void ReadFile(std::ifstream& file, T& v)
	{
		file.read(reinterpret_cast<char*>(&v), sizeof(T));
	}

	template<class T>
	void ReadFile(std::ifstream& file, std::vector<T>& v)
	{
		uint32 size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

		if (size == 0)
			return;
		v.resize(size);
		file.read(reinterpret_cast<char*>(&v[0]), sizeof(T) * size);
	}
	void ReadFile(std::ifstream& file, std::string& str)
	{
		uint32 size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

		if (size == 0)
			return;

		char buff[512];
		memset(buff, 0, 512);
		file.read(buff, sizeof(char) * size);
		str = std::string(buff);
	}

	void ReadFile(std::ifstream& file, GameEngine::Animation::JointTransform& transform)
	{
		ReadFile(file, transform.position);
		ReadFile(file, transform.rotation);
	}

	void ReadFile(std::ifstream& file, GameEngine::Animation::KeyFrame& keyFrame)
	{
		ReadFile(file, keyFrame.timeStamp);

		uint32 size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

		if (size > 0)
		{
			for (uint32 i = 0; i < size; ++i)
			{
				std::string name;
				GameEngine::Animation::JointTransform transform;
				ReadFile(file, name);
				ReadFile(file, transform);
				keyFrame.transforms[name] = transform;
			}
		}

		file.read(reinterpret_cast<char*>(&size), sizeof(uint32));
		if (size == 0)
			return;

		for (uint32 i = 0; i < size; ++i)
		{
			uint32 id;
			GameEngine::Animation::JointTransform transform;
			ReadFile(file, id);
			ReadFile(file, transform);
			keyFrame.idTransforms_[id] = transform;
		}
	}

	void ReadFile(std::ifstream& file, GameEngine::Animation::AnimationClip& clip)
	{
		ReadFile(file, clip.name);
		float correctLength = 0;
		ReadFile(file, correctLength);

		uint32 framesSize = 0;
		file.read(reinterpret_cast<char*>(&framesSize), sizeof(uint32));

		if (framesSize == 0)
			return;

		for (uint32 i = 0; i < framesSize; ++i)
		{
			GameEngine::Animation::KeyFrame keyFrame;
			ReadFile(file, keyFrame);
			clip.AddFrame(keyFrame);
		}

		if (fabs(correctLength - clip.GetLength()) < std::numeric_limits<float>::epsilon())
		{
			Error("Animation length is diffrent.");
		}

	}

	void ReadFile(std::ifstream& file, AnimationClipsMap& animations)
	{
		uint32 size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

		if (size == 0)
			return;

		for (uint32 i = 0; i < size; ++i)
		{
			std::string name;
			ReadFile(file, name);
			ReadFile(file, animations[name]);
		}
	}

	void ReadFile(std::ifstream& file, GameEngine::Animation::Joint& skeleton)
	{
		ReadFile(file, skeleton.id);
		ReadFile(file, skeleton.size);
		ReadFile(file, skeleton.name);
		ReadFile(file, skeleton.transform);
		ReadFile(file, skeleton.invtransform);
		ReadFile(file, skeleton.animatedTransform);

		uint32 size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

		if (size == 0)
			return;

		for (uint32 i =0; i < size; ++i)
		{
			skeleton.children.emplace_back();
			ReadFile(file, skeleton.children.back());
		}
	}


	std::unique_ptr<CModel> AbstractLoader::ReadBinFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::binary | std::ios::in);

		if (!file.is_open())
			return nullptr;

		auto out = std::make_unique<CModel>();

		uint32 meshesSize = 0;
		std::string file_name;
		ReadFile(file, file_name);
		Log("file : " + file_name);
		ReadFile(file, meshesSize);

		for (uint32 m = 0; m < meshesSize; ++m)
		{
			std::string name;
			ReadFile(file, name);

			SMaterial material;
			ReadFile(file, material.name);
			ReadFile(file, material.ambient);
			ReadFile(file, material.diffuse);
			ReadFile(file, material.specular);
			ReadFile(file, material.shineDamper);
			ReadFile(file, material.indexOfRefraction);
			ReadFile(file, material.reflectivity);
			ReadFile(file, material.isTransparency);
			ReadFile(file, material.useFakeLighting);

			std::string textures[4];
			ReadFile(file, textures[0]);
			ReadFile(file, textures[1]);
			ReadFile(file, textures[2]);
			ReadFile(file, textures[3]);

			material.diffuseTexture = textureLoader_.LoadTexture(textures[0], true, true, ObjectTextureType::MATERIAL);
			material.normalTexture = textureLoader_.LoadTexture(textures[1], true, true, ObjectTextureType::MATERIAL);
			material.ambientTexture = textureLoader_.LoadTexture(textures[2], true, true, ObjectTextureType::MATERIAL);
			material.specularTexture = textureLoader_.LoadTexture(textures[3], true, true, ObjectTextureType::MATERIAL);

			auto& mesh = *(out->AddMesh());
			auto& data = mesh.GetMeshDataRef();

			ReadFile(file, data.bitangents_);
			ReadFile(file, data.bonesWeights_);
			ReadFile(file, data.indices_);
			ReadFile(file, data.instancedMatrixes_);
			ReadFile(file, data.joinIds_);
			ReadFile(file, data.normals_);
			ReadFile(file, data.positions_);
			ReadFile(file, data.tangents_);
			ReadFile(file, data.textCoords_);
			mat4 transform;
			ReadFile(file, transform);
			mesh.SetTransformMatrix(transform);
		}

		file.close();

		return out;
	}

	AbstractLoader::AbstractLoader(GameEngine::IGraphicsApiPtr graphicsApi, CTextureLoader& textureLoader)
		: graphicsApi_(graphicsApi)
		, textureLoader_(textureLoader)
		, loadedFromBin_(false)
	{
	}
	void AbstractLoader::Parse(const std::string& filename)
	{
		Utils::GetFileAndPath(filename, fileName_, filePath_);

		auto binFile = CreateBinPath(fileName_);
		if (EngineConf.enableBinaryLoading && Utils::CheckFileExist(binFile))
		{
			ReadBinFile(binFile);
			loadedFromBin_ = true;
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
			CreateBinFile(newModel);
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
	void AbstractLoader::CreateBinFile(const std::unique_ptr<CModel>& model)
	{
		if (!EngineConf.enableBinaryLoading)
			return;

		std::fstream binaryFile(CreateBinPath(fileName_), std::ios::binary | std::ios::out);

		WriteToFile(binaryFile, model->GetFileName());
		WriteToFile(binaryFile, model->GetMeshes().size());
		for (auto& mesh : model->GetMeshes())
		{
			WriteToFile(binaryFile, mesh.GetCMeshDataRef(), mesh.GetMaterial(), mesh.GetMeshTransform());
		}
		WriteToFile(binaryFile, model->GetMeshes().size());
		WriteToFile(binaryFile, model->animationClips_);
		WriteToFile(binaryFile, model->skeleton_);

		binaryFile.close();
	}
	std::unique_ptr<CModel> AbstractLoader::CreateModelFromBin()
	{
		return ReadBinFile(fileName_);
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
