#include "MyModel.h"
#include <algorithm>
#include <sstream>
#include "../../../Utils/ParseUtils.h"

CMyModel::CMyModel(CTextureLoader & texture_lodaer)
	: m_TextureLodaer(texture_lodaer)
{
}

CMyModel::~CMyModel()
{
	Log("Destructor my model : " + m_Filename);
}

void CMyModel::InitModel(const std::string & file_name)
{	
	ParseObjFile(file_name);
	AddFinalMeshes();
	CModel::InitModel(file_name);
}
void CMyModel::ParseObjFile(const std::string& file_name)
{
	auto file = Utils::ReadFile(file_name);
	std::string path, filename;
	Utils::GetFileAndPath(file_name, filename, path);

	std::replace(file.begin(), file.end(), '\t', ' ');

	std::istringstream f(file);

	std::string line;

	WBLoader::Mesh* current_mesh = nullptr;
	WBLoader::Object* current_object = nullptr;	

	while (std::getline(f, line))
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
//Example Material file
//# Blender MTL File : 'None'
//# Material Count : 3
//
//newmtl Material
//Ns 96.078431
//Ka 1.000000 1.000000 1.000000
//Kd 0.640000 0.001047 0.492338
//Ks 0.500000 0.500000 0.500000
//Ke 0.000000 0.000000 0.000000
//Ni 1.000000
//d 1.000000
//illum 2
void CMyModel::ReadMaterialFile(const std::string& file_name)
{
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
			if(current_material != nullptr)
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
				current_material->m_DiffuseTexture = m_TextureLodaer.LoadTexture("../Data/Textures/" + value, true, true, TextureType::MATERIAL);
		}
		if (!prefix.compare("map_bump") || !prefix.compare("map_Bump"))
		{
			if (current_material != nullptr)
				current_material->m_NormalTexture = m_TextureLodaer.LoadTexture("../Data/Textures/" + value, true, true, TextureType::MATERIAL);
		}
	}

}

void CMyModel::ProcesVertex(WBLoader::Object * object, const std::string& value)
{	
	if (object == nullptr)
	{
		auto err = "[Error] parsing obj file. " + m_Filename + "\n";
		throw std::runtime_error(err.c_str());
	}
	auto v = Get::Vector3d(value, ' ');
	//glm::vec3 v;
	//sscanf_s(value.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
	vertex.push_back(v);
}

void CMyModel::ProcesNormal(WBLoader::Object * object, const std::string & value)
{	
	if (object == nullptr)
	{
		auto err = "[Error] parsing obj file. " + m_Filename + "\n";
		throw std::runtime_error(err.c_str());
	}
	auto v = Get::Vector3d(value, ' ');
	//glm::vec3 v;
	//sscanf_s(value.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
	normals.push_back(v);
}

void CMyModel::ProcesUVs(WBLoader::Object * object, const std::string & value)
{	
	if (object == nullptr)
	{
		auto err = "[Error] parsing obj file. " + m_Filename + "\n";
		throw std::runtime_error(err.c_str());
	}
	auto v = Get::Vector2d(value, ' ');
	//glm::vec2 v;
	//sscanf_s(value.c_str(), "%f %f", &v.x, &v.y);
	text_coords.push_back(v);	
}

void CMyModel::ProcesFace(WBLoader::Object * object, WBLoader::Mesh* mesh, const std::string & value)
{
	if (object == nullptr && mesh == nullptr)
	{
		auto err = "[Error] parsing obj file. " + m_Filename + "\n";
		throw std::runtime_error(err.c_str());
	}
	
	/*wb::vec3i f1, f2, f3;
	sscanf_s(value.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d",
		&f1.x, &f1.y, &f1.z, 
		&f2.x, &f2.y, &f2.z, 
		&f3.x, &f3.y, &f3.z);
	
	f1.x -= 1;
	f1.y -= 1;
	f1.z -= 1;

	f2.x -= 1;
	f2.y -= 1;
	f2.z -= 1;

	f3.x -= 1;
	f3.y -= 1;
	f3.z -= 1;

	WBLoader::VertexBuffer vb;
	vb.indexes = f1;
	vb.position = vertex[f1.x];
	vb.uvs = text_coords[f1.y];
	vb.normal = normals[f1.z];
	mesh->vertexBuffer.push_back(vb);

	vb.indexes = f2;
	vb.position = vertex[f2.x];
	vb.uvs = text_coords[f2.y];
	vb.normal = normals[f2.z];
	mesh->vertexBuffer.push_back(vb);

	vb.indexes = f3;
	vb.position = vertex[f3.x];
	vb.uvs = text_coords[f3.y];
	vb.normal = normals[f3.z];
	mesh->vertexBuffer.push_back(vb);
	return;*/

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

void CMyModel::ProcesObject(WBLoader::Object *& object, const std::string & value)
{
	objects.push_back(WBLoader::Object());
	object = &objects.back();
	object->name = value;
}

void CMyModel::ProcesMaterial(WBLoader::Object * object, WBLoader::Mesh *& mesh, const std::string & value)
{
	if (object == nullptr)
	{
		auto err = "[Error] parsing obj file. " + m_Filename + "\n";
		throw std::runtime_error(err.c_str());
	}

	object->meshes.push_back(WBLoader::Mesh());
	mesh = &objects.back().meshes.back();	

	auto f = [&](const SMaterial& v) {
		return value == v.name;
	};
	auto material = std::find_if(std::begin(materials), std::end(materials), f);
	if(material != std::end(materials))
		mesh->material = *material;
}

void CMyModel::AddFinalMeshes()
{
	SBonesInfo m_BonesInfo;
	for (auto& obj : objects)
	{
		for (auto& mesh : obj.meshes)
		{
			mesh.IndexinVBO();
			AddMesh(mesh.fpostions, mesh.fuvs, mesh.fnormal, mesh.ftangents, mesh.indices, mesh.material, m_BonesInfo.bones);
		}
	}
}


namespace WBLoader
{
	int WBLoader::Mesh::FindIndex(const std::list<wb::vec3i>& vertexes, const wb::vec3i & v)
	{
		auto it = std::find(vertexes.begin(), vertexes.end(), v);
		auto index = std::distance(vertexes.begin(), it);

		if (it != vertexes.end())
			return index;
		else
			return -1;
	}

	int WBLoader::Mesh::FindIndexFast(std::map<wb::vec3i, unsigned short>& vertexes, const wb::vec3i & v)
	{
		auto it = vertexes.find(v);
		if (it == vertexes.end())
			return -1;
		return it->second;
	}

	void WBLoader::Mesh::AddVec3ToFloatBuffer(std::vector<float>& buffer, const glm::vec3 & v)
	{
		buffer.push_back(v.x);
		buffer.push_back(v.y);
		buffer.push_back(v.z);
	}

	void WBLoader::Mesh::AddVec2ToFloatBuffer(std::vector<float>& buffer, const glm::vec2 & v)
	{
		buffer.push_back(v.x);
		buffer.push_back(v.y);
	}

	void WBLoader::Mesh::IndexinVBO()
	{
		computeTangentBasis();

		//std::list<wb::vec3i> out_indexes;
		std::map<wb::vec3i, unsigned short> out_indexes;
		for (auto& v : vertexBuffer)
		{
			//auto i = FindIndex(out_indexes, v.indexes);
			auto i = FindIndexFast(out_indexes, v.indexes);
			if (i >= 0)
			{
				indices.push_back(static_cast<unsigned short>(i));

				ftangents[3 * i] = v.tangents.x;
				ftangents[3 * i + 1] = v.tangents.y;
				ftangents[3 * i + 2] = v.tangents.z;


				fbitangents[3 * i] = v.bitangents.x;
				fbitangents[3 * i + 1] = v.bitangents.y;
				fbitangents[3 * i + 2] = v.bitangents.z;

			}
			else
			{
				AddVec3ToFloatBuffer(fpostions, v.position);
				AddVec3ToFloatBuffer(fnormal, v.normal);
				AddVec2ToFloatBuffer(fuvs, v.uvs);
				AddVec3ToFloatBuffer(ftangents, v.tangents);
				AddVec3ToFloatBuffer(fbitangents, v.bitangents);
				auto newIndex = (unsigned short)out_indexes.size();
				//out_indexes.push_back(v.indexes);
				out_indexes[v.indexes] = newIndex;
				indices.push_back(newIndex);
			}
		}
	}

	void WBLoader::Mesh::computeTangentBasis()
	{
		for (uint i = 0; i < vertexBuffer.size(); i += 3) {

			// Shortcuts for vertices
			glm::vec3 & v0 = vertexBuffer[i + 0].position;
			glm::vec3 & v1 = vertexBuffer[i + 1].position;
			glm::vec3 & v2 = vertexBuffer[i + 2].position;

			// Shortcuts for UVs
			glm::vec2 & uv0 = vertexBuffer[i + 0].uvs;
			glm::vec2 & uv1 = vertexBuffer[i + 1].uvs;
			glm::vec2 & uv2 = vertexBuffer[i + 2].uvs;

			// Edges of the triangle : postion delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			// Set the same tangent for all three vertices of the triangle.
			// They will be merged later, in vboindexer.cpp
			vertexBuffer[i + 0].tangents = tangent;
			vertexBuffer[i + 1].tangents = tangent;
			vertexBuffer[i + 2].tangents = tangent;

			vertexBuffer[i + 0].bitangents = bitangent;
			vertexBuffer[i + 1].bitangents = bitangent;
			vertexBuffer[i + 2].bitangents = bitangent;
		}

		// See "Going Further"
		for (uint i = 0; i < vertexBuffer.size(); i += 1)
		{
			glm::vec3 & n = vertexBuffer[i].normal;
			glm::vec3 & t = vertexBuffer[i].tangents;
			glm::vec3 & b = vertexBuffer[i].bitangents;

			// Gram-Schmidt orthogonalize
			t = glm::normalize(t - n * glm::dot(n, t));

			// Calculate handedness
			if (glm::dot(glm::cross(n, t), b) < 0.0f) {
				t = t * -1.0f;
			}
		}


	}
}