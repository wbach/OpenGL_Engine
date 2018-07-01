#include "BinaryReader.h"
#include "Types.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Animations/KeyFrame.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/TextureLoader.h"
#include <Logger/Log.h>
#include <fstream>

namespace WBLoader
{
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
	void ReadFile(std::ifstream & file, std::string & str)
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
	void ReadFile(std::ifstream & file, GameEngine::Animation::JointTransform & transform)
	{
		ReadFile(file, transform.position);
		ReadFile(file, transform.rotation);
	}
	void ReadFile(std::ifstream & file, GameEngine::Animation::Joint & skeleton)
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

	for (uint32 i = 0; i < size; ++i)
	{
		skeleton.children.emplace_back();
		ReadFile(file, skeleton.children.back());
	}
}
	std::unique_ptr<CModel> ReadBinFile(const std::string& filename, CTextureLoader& textureLoader)
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

			material.diffuseTexture = textureLoader.LoadTexture(textures[0], true, true, ObjectTextureType::MATERIAL);
			material.normalTexture = textureLoader.LoadTexture(textures[1], true, true, ObjectTextureType::MATERIAL);
			material.ambientTexture = textureLoader.LoadTexture(textures[2], true, true, ObjectTextureType::MATERIAL);
			material.specularTexture = textureLoader.LoadTexture(textures[3], true, true, ObjectTextureType::MATERIAL);

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
} // WBLoader
