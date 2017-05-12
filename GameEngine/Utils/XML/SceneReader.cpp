#include "SceneReader.h"
#include <rapidxml.hpp>
#include "../Utils.h"
#include "../../Debug_/Log.h"
#include "../../Objects/GameObject.h"
#include "../../Scene/Scene.hpp"

using namespace rapidxml;

float ToFloat(const std::string& s)
{
	float x = 0.f;
	try
	{
		x = std::stof(s);
	}
	catch (const std::invalid_argument& e)
	{
		std::string er = e.what();
		Error("ParseVector3d : " + er);
	}
	catch (const std::out_of_range& e)
	{
		std::string er = e.what();
		Error("ParseVector3d : " + er);
	}
	return x;
}

glm::vec3 ParseVector3d(xml_node<>* root)
{
	glm::vec3 vec;
	for (auto node = root->first_node(); node; node = node->next_sibling())
	{
		std::string name = node->name();
		std::string value = node->value();
		if (name == "x")
			vec.x = ToFloat(value);
		else if (name == "y")
			vec.y = ToFloat(value);
		else if (name == "z")
			vec.z = ToFloat(value);
	}
	return vec;
}

CGameObject* ParseTerrain(xml_node<>* root)
{
	return nullptr;
}
CModel* ParseModel(xml_node<>* node)
{
	return nullptr;
}
CGameObject* ParseEntity(xml_node<>* root)
{
	/*CEntity* entity = new CEntity();

	for (auto node = root->first_node(); node; node = node->next_sibling())
	{
		std::string name = node->name();
		std::string value = node->value();

		if (name == "Position")
			entity->m_WorldTransform.SetPosition(ParseVector3d(node));
		if (name == "Scale")
			entity->m_WorldTransform.SetScale(ParseVector3d(node));
		if (name == "Rotation")
			entity->m_WorldTransform.SetRotation(ParseVector3d(node));
		if (name == "Model")
			entity->GetModel()
	}*/
	return nullptr;
}

void ParseScene(CScene* scene, xml_node<>* root)
{
	for (auto node = root->first_node(); node; node = node->next_sibling())
	{
		std::string name = node->name();
		std::string value = node->value();

		if (name == "Entity")
			scene->AddGameObject(ParseEntity(node));
		else if (name == "Terrain")
			scene->AddGameObject(ParseTerrain(node));
	}
}

void SceneReader::ReadScene(CScene * scene, const std::string & filename)
{
	auto data = Utils::ReadFile(filename);

	xml_document<> document;

	try
	{
		document.parse<0>(const_cast<char*>(data.c_str()));
	}
	catch (...)
	{
		Error("Can not parse file " + filename);
		return;
	}

	ParseScene(scene, document.first_node());
}
