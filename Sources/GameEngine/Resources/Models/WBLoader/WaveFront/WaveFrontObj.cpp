#include "WaveFrontObj.h"
#include "../MeshData.h"
#include "../../Mesh.h"
#include "../../../TextureLoader.h"
#include "../../../../Engine/Configuration.h"

#include "ParseUtils.h"

#include <algorithm>

namespace WBLoader
{
	WaveFrontObjLoader::WaveFrontObjLoader(CTextureLoader & textureLodaer)
		: textureLodaer(textureLodaer)
	{

	}
	void WaveFrontObjLoader::ParseFile(const std::string & filename)
	{
		GetFileData(filename);

		if (fileData.empty())
			return;

		ProcessFileData();
	}

	bool WaveFrontObjLoader::CheckExtension(const std::string & filename)
	{
		auto ext = Utils::GetFileExtension(filename);
		return ext == "obj" || ext == "OBJ" || ext == "Obj";
	}

	void WaveFrontObjLoader::GetFileData(const std::string & file_name)
	{
		fileData = Utils::ReadFileLines(file_name);

		if (fileData.empty())
			return;

		Utils::GetFileAndPath(file_name, filename, path);		
	}
	void WaveFrontObjLoader::ProcessFileData()
	{
		WBLoader::Mesh* current_mesh = nullptr;
		WBLoader::Object* current_object = nullptr;

		for(const auto& line : fileData)
		{
			auto substr = line.substr(line.find_first_not_of(' '));
			auto prefix = substr.substr(0, substr.find_first_of(' '));
			auto value = substr.substr(substr.find_first_of(' ') + 1);

			if (!prefix.compare("mtllib"))
				ReadMaterialFile(path + "/" + value);

			if (!prefix.compare("o"))
				ProcesObject(current_object, value);

			if (!prefix.compare("v"))
				ProcesVertex(current_object, value);

			if (!prefix.compare("vt"))
				ProcesUVs(current_object, value);

			if (!prefix.compare("vn"))
				ProcesNormal(current_object, value);

			if (!prefix.compare("usemtl"))
				ProcesMaterial(current_object, current_mesh, value);

			if (!prefix.compare("f"))
				ProcesFace(current_object, current_mesh, value);
		}
	}

	void WaveFrontObjLoader::ReadMaterialFile(const std::string& file_name)
	{
		EngineConf_AddRequiredFile(file_name);

		auto file = Utils::ReadFile(file_name);

		std::istringstream f(file);
		std::string line;
		SMaterial* current_material = nullptr;
		while (std::getline(f, line))
		{
			if (line.empty())
				continue;
			auto substr = line.substr(line.find_first_not_of(' '));
			auto prefix = substr.substr(0, substr.find_first_of(' '));
			auto value = substr.substr(substr.find_first_of(' ') + 1);

			if (!prefix.compare("newmtl"))
			{
				materials.push_back(SMaterial());
				current_material = &materials.back();
				current_material->name = value;
			}
			if (!prefix.compare("Ns"))
			{
				if (current_material != nullptr)
					current_material->shineDamper = std::stof(value);
			}
			if (!prefix.compare("Ka"))
			{
				if (current_material != nullptr)
					current_material->ambient = Get::Vector3d(value, ' ');
			}
			if (!prefix.compare("Kd"))
			{
				if (current_material != nullptr)
					current_material->diffuse = Get::Vector3d(value, ' ');
			}
			if (!prefix.compare("Ks"))
			{
				if (current_material != nullptr)
					current_material->specular = Get::Vector3d(value, ' ');
			}
			if (!prefix.compare("Ke"))
			{
				//material.ke = Get::Vector3d(value, ' ');
			}
			if (!prefix.compare("Ni"))
			{
				//material.Ni = std::stof(value);
			}
			if (!prefix.compare("d"))
			{
				//material.d = std::stof(value);
			}
			if (!prefix.compare("illum"))
			{
				//material.illum = std::stoi(value);		
			}
			if (!prefix.compare("map_Kd"))
			{
				if (current_material != nullptr)
					current_material->diffuseTexture = textureLodaer.LoadTexture("Textures/" + value, true, true, TextureType::MATERIAL);
			}
			if (!prefix.compare("map_bump") || !prefix.compare("map_Bump"))
			{
				if (current_material != nullptr)
					current_material->normalTexture = textureLodaer.LoadTexture("Textures/" + value, true, true, TextureType::MATERIAL);
			}
		}

	}

	void WaveFrontObjLoader::ProcesVertex(WBLoader::Object * object, const std::string& value)
	{
		if (object == nullptr)
		{
			auto err = "[Error] parsing obj file. " + filename + "\n";
			throw std::runtime_error(err.c_str());
		}
		auto v = Get::Vector3d(value, ' ');
		vertex.push_back(v);
	}

	void WaveFrontObjLoader::ProcesNormal(WBLoader::Object * object, const std::string & value)
	{
		if (object == nullptr)
		{
			auto err = "[Error] parsing obj file. " + filename + "\n";
			throw std::runtime_error(err.c_str());
		}
		auto v = Get::Vector3d(value, ' ');
		normals.push_back(v);
	}

	void WaveFrontObjLoader::ProcesUVs(WBLoader::Object * object, const std::string & value)
	{
		if (object == nullptr)
		{
			auto err = "[Error] parsing obj file. " + filename + "\n";
			throw std::runtime_error(err.c_str());
		}
		auto v = Get::Vector2d(value, ' ');
		text_coords.push_back(v);
	}

	void WaveFrontObjLoader::ProcesFace(WBLoader::Object * object, WBLoader::Mesh* mesh, const std::string & value)
	{
		if (object == nullptr && mesh == nullptr)
		{
			auto err = "[Error] parsing obj file. " + filename + "\n";
			throw std::runtime_error(err.c_str());
		}

		//		2//1 4//1 1//1
		auto tokens = Utils::SplitString(value, ' ');
		if (tokens.size() > 3) return;

		for (auto& t : tokens)
		{
			WBLoader::VertexBuffer vb;
			//		2//1
			auto vf = Utils::SplitString(t, '/');
			int x = 0;
			for (auto& i : vf)
			{
				int ii = 0;
				if (!i.empty())
				{
					ii = std::stoi(i) - 1;
				}

				if (x == 0)
				{
					if (!i.empty())
					{
						vb.indexes.x = ii;
						vb.position = vertex[ii];
					}
				}
				if (x == 1)
				{
					if (!i.empty())
					{
						vb.indexes.y = ii;
						vb.uvs = text_coords[ii];
					}

				}
				if (x == 2)
				{
					if (!i.empty())
					{
						vb.indexes.z = ii;
						vb.normal = normals[ii];
					}
				}
				x++;
			}
			mesh->vertexBuffer.push_back(vb);
		}
	}

	void WaveFrontObjLoader::ProcesObject(WBLoader::Object *& object, const std::string & value)
	{
		objects.push_back(WBLoader::Object());
		object = &objects.back();
		object->name = value;
	}

	void WaveFrontObjLoader::ProcesMaterial(WBLoader::Object * object, WBLoader::Mesh *& mesh, const std::string & value)
	{
		if (object == nullptr)
		{
			auto err = "[Error] parsing obj file. " + filename + "\n";
			throw std::runtime_error(err.c_str());
		}

		object->meshes.push_back(WBLoader::Mesh());
		mesh = &objects.back().meshes.back();

		auto f = [&](const SMaterial& v) {
			return value == v.name;
		};
		auto material = std::find_if(std::begin(materials), std::end(materials), f);
		if (material != std::end(materials))
			mesh->material = *material;
	}

}