#include "ColladaDae.h"
#include "../../../../Utils/Utils.h"
#include "../../../../Utils/XML/XMLUtils.h"
#include "../../../../Engine/Configuration.h"
#include <rapidxml.hpp>

namespace WBLoader
{
	ColladaDae::ColladaDae(CTextureLoader & textureLodaer)
		: textureLodaer(textureLodaer)
	{
	}
	void ColladaDae::ParseFile(const std::string & filename)
	{
		auto file_name = EngineConf.dataFilesLocation + filename;

		parsingFileName = filename;
		fileData = Utils::ReadFile(file_name);

		if (fileData.empty())
			return;

		rapidxml::xml_document<>document;

		try
		{
			document.parse<0>(const_cast<char*>(fileData.c_str()));
		}
		catch (...)
		{
			Error("Can not parse file " + filename);
			return;
		}

		ProccesColladaNode(document);		
	}
	std::list<CMesh> ColladaDae::CreateFinalMesh()
	{
		std::list<CMesh> out;

		SBonesInfo m_BonesInfo;
		for (auto& obj : objects)
		{
			for (auto& mesh : obj.meshes)
			{
				mesh.IndexinVBO();
				//output.emplace_back(mesh.material, mesh.fpostions, mesh.fuvs, mesh.fnormal, mesh.ftangents, mesh.indices, m_BonesInfo.bones);
				out.emplace_back(mesh.material, mesh.fpostions, mesh.fuvs, mesh.fnormal, mesh.ftangents, mesh.indices, m_BonesInfo.bones);
			}
		}
		return out;
	}
	bool ColladaDae::CheckExtension(const std::string & filename)
	{
		auto ext = Utils::GetFileExtension(filename);
		return ext == "dae" || ext == "DAE" || ext == "Dae";
	}
	void ColladaDae::ProccesColladaNode(const rapidxml::xml_document<char>& document)
	{
		auto root = document.first_node();

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "library_geometries")
				ProccesLibraryGeometryNode(snode);
		}
	}
	void ColladaDae::ProccesLibraryGeometryNode(rapidxml::xml_node<char>* root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "geometry")
				objects.push_back(ProccesGeometryNode(snode));
		}
	}
	WBLoader::Object ColladaDae::ProccesGeometryNode(rapidxml::xml_node<char>* root)
	{
		WBLoader::Object out;

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "mesh")
				out.meshes.push_back(ProccesMeshNode(snode));
		}

		return out;
	}
	WBLoader::Mesh ColladaDae::ProccesMeshNode(rapidxml::xml_node<char>* root)
	{
		WBLoader::Mesh out;

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "source")
				ProccesMeshSourceNode(snode, out);
			if (node_data.name == "polylist")
				ProccesPolyListNode(snode, out);
		}
		return out;

	}
	void ColladaDae::ProccesMeshSourceNode(rapidxml::xml_node<char>* root, WBLoader::Mesh& out_mesh)
	{
		ColladaTypes::ArrayType type;

		auto att = root->first_attribute("id");

		if (att == 0)
		{
			Log(parsingFileName + "::" + "attribute 'id' dosent exist in SourceNode");
			return;
		}

		auto node_data = Utils::GetRapidAttributeData(att);
		if (node_data.name == "id")
			type = GetArrayType(node_data.value);

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "float_array")
			{
				switch (type)
				{
				case ColladaTypes::ArrayType::POSITIONS:
					out_mesh.vertex = GetVectors3dFromString(node_data.value);
					break;
				case ColladaTypes::ArrayType::NORMALS:
					out_mesh.normals = GetVectors3dFromString(node_data.value);
					break;
				case ColladaTypes::ArrayType::TEXT_COORD:
					out_mesh.text_coords = GetVectors2dFromString(node_data.value);
					break;
				}
			}			
		}
	}

	void ColladaDae::ProccesPolyListNode(rapidxml::xml_node<char>* root, WBLoader::Mesh & out_mesh)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "p")
			{
				auto indices = GetIntsFromString(node_data.value);

				for (uint32 x = 0; x < indices.size(); x += 3)
				{
					WBLoader::VertexBuffer vb;

					vb.position = out_mesh.vertex[indices[x]];
					vb.normal = out_mesh.normals[indices[x + 1]];
					vb.indexes.x = indices[x];
					vb.indexes.y = indices[x + 1];
					vb.indexes.z = indices[x + 2];

					out_mesh.vertexBuffer.push_back(vb);
				}
			}
		}
	}

	std::vector<glm::vec2> ColladaDae::GetVectors2dFromString(const std::string & str) const
	{
		std::vector<glm::vec2> out;

		auto strs = Utils::SplitString(str, ' ');

		std::vector<float> tmp;

		for (const auto& str : strs)
		{
			auto a = Utils::StringToFloat(str);
			tmp.push_back(a);

			if (tmp.size() >= 2)
			{
				glm::vec2 v(tmp[0], tmp[1]);
				out.push_back(v);
				tmp.clear();
			}
		}

		return out;
	}

	std::vector<glm::vec3> ColladaDae::GetVectors3dFromString(const std::string & str) const
	{
		std::vector<glm::vec3> out;

		auto strs = Utils::SplitString(str, ' ');

		std::vector<float> tmp;

		for (const auto& str : strs)
		{
			auto a = Utils::StringToFloat(str);
			tmp.push_back(a);

			if (tmp.size() >= 3)
			{
				glm::vec3 v(tmp[0], tmp[1], tmp[2]);
				out.push_back(v);
				tmp.clear();
			}
		}

		return out;
	}

	std::vector<float> ColladaDae::GetFloatsFromString(const std::string& str) const
	{
		std::vector<float> out;

		auto strs = Utils::SplitString(str, ' ');

		for (const auto& str : strs)
		{
			auto a = Utils::StringToFloat(str);
			out.push_back(a);
		}

		return out;
	}

	std::vector<uint16> ColladaDae::GetIntsFromString(const std::string & str) const
	{
		std::vector<uint16> out;

		auto strs = Utils::SplitString(str, ' ');

		for (const auto& str : strs)
		{
			auto a = Utils::StringToInt(str);
			out.push_back(a);
		}
		return out;
	}

	ColladaTypes::ArrayType ColladaDae::GetArrayType(const std::string & str) const
	{
		auto pos = str.find("positions");
		if (pos != std::string::npos)
			return ColladaTypes::ArrayType::POSITIONS;

		auto normal = str.find("normals");
		if (normal != std::string::npos)
			return ColladaTypes::ArrayType::NORMALS;

		auto text_coords = str.find("map");
		if (text_coords != std::string::npos)
			return ColladaTypes::ArrayType::TEXT_COORD;

		return ColladaTypes::ArrayType::UNKNOWN;
	}
}