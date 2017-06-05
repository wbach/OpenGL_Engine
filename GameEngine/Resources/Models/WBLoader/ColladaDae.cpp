#include "ColladaDae.h"
#include "../../../Utils/XML/XMLUtils.h"
#include <rapidxml.hpp>

namespace WBLoader
{
	ColladaDae::ColladaDae(CTextureLoader & textureLodaer)
		: textureLodaer(textureLodaer)
	{
	}
	void ColladaDae::ParseFile(const std::string & filename)
	{
		fileData = Utils::ReadFile(filename);
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
		return std::list<CMesh>();
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
				ProccesGeometryNode(snode);
		}
	}
	void ColladaDae::ProccesGeometryNode(rapidxml::xml_node<char>* root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "mesh")
				ProccesMeshNode(snode);
		}
	}
	void ColladaDae::ProccesMeshNode(rapidxml::xml_node<char>* root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "source")
				ProccesMeshNode(snode);
		}
	}
	void ColladaDae::ProccesMeshSourceNode(rapidxml::xml_node<char>* root)
	{
		for (auto att = root->first_attribute(); att; att = att->next_attribute())
		{
			auto node_data = Utils::GetRapidAttributeData(att);
			//if (node_data.name == "id")
		
		}
	}
	ColladaDae::ArrayType ColladaDae::GetArrayType(const std::string & str) const
	{
		auto pos = str.find("positions");
		if (pos != std::string::npos)
			return ColladaDae::ArrayType::POSITIONS;

		auto normal = str.find("normals");
		if (normal != std::string::npos)
			return ColladaDae::ArrayType::NORMALS;

		return ColladaDae::ArrayType::UNKNOWN;
	}
}