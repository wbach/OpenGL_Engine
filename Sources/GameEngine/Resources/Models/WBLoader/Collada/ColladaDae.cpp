#include "ColladaDae.h"
#include "GameEngine/Resources/TextureLoader.h"

#include "GLM/GLMUtils.h"
#include "ParseUtils.h"
#include "XML/XMLUtils.h"
#include "Logger/Log.h"
#include "Utils.h"

#include <rapidxml.hpp>
#include <algorithm>

namespace WBLoader
{
	ColladaDae::ColladaDae(CTextureLoader& textureLodaer)
		: textureLodaer(textureLodaer)
	{
	}

	void ColladaDae::ParseFile(const std::string& filename)
	{
		auto file_name = filename;

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
	void ColladaDae::ProccesColladaNode(const XMLDocument& document)
	{
		auto root = document.first_node();

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "library_geometries")
				ProccesLibraryGeometryNode(snode);
			if (node_data.name == "library_visual_scenes")
				ProccesLibraryVisualScenesNode(snode);
			if (node_data.name == "library_images")
				ProccesLibraryImages(snode);
			if (node_data.name == "library_effects")
				ProccesLibraryEffects(snode);
			if (node_data.name == "library_controllers")
				ProccesLibraryControllers(snode);
		}
	}
	bool ColladaDae::CheckExtension(const std::string& filename)
	{
		auto ext = Utils::GetFileExtension(filename);
		return ext == "dae" || ext == "DAE" || ext == "Dae";
	}
	void ColladaDae::ProccesLibraryImages(XMLNode* root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "image")
				ProccesImage(snode);
		}
	}
	void ColladaDae::ProccesLibraryGeometryNode(XMLNode* root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "geometry")
				objects.push_back(ProccesGeometryNode(snode));
		}
	}
	void ColladaDae::ProccesLibraryVisualScenesNode(XMLNode* root)
	{
		if (objects.empty())
		{
			Error(parsingFileName + " : No geometry objects but parse visual scene node");
			return;
		}

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "visual_scene")
				ProccesVisualScebeNode(snode);
		}
	}
	void ColladaDae::ProccesLibraryControllers(XMLNode * root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "controller")
				ProccesController(snode);
		}
	}
	void ColladaDae::ProccesImage(XMLNode * root)
	{
		auto id_attribute = root->first_attribute("id");
		if (id_attribute == 0)
		{
			Log(parsingFileName + " : ColladaDae::ProccesImage image id attriubte not found");
			return;
		}

		auto att_data = Utils::GetRapidAttributeData(id_attribute);

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "init_from")
				texturesMap[att_data.value] = textureLodaer.LoadTexture("Textures/" + node_data.value, true, true, TextureType::MATERIAL);
		}
	}
	void ColladaDae::ProccesLibraryEffects(XMLNode* root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "effect")
				ProccesEffect(snode);
		}
	}
	void ColladaDae::ProccesEffect(XMLNode * root)
	{
		auto id_attribute = root->first_attribute("id");
		if (id_attribute == 0)
		{
			Log("ColladaDae::ProccesEffect material not found");
			return;
		}

		auto att_data = Utils::GetRapidAttributeData(id_attribute);
		auto material_name = GetName(att_data.value, "-effect");

		materialMap[material_name].name = material_name;

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "profile_COMMON")
				ProccesProfileCommon(snode, material_name);
		}
	}
	void ColladaDae::ProccesProfileCommon(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "newparam")
				ProccesNewParam(snode);
			if (node_data.name == "technique")
				ProccesTechnique(snode, material_name);
		}
	}
	void ColladaDae::ProccesTechnique(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "phong")
				ProccesPhong(snode, material_name);
		}
	}
	void ColladaDae::ProccesNewParam(XMLNode * root)
	{
	}
	void ColladaDae::ProccesPhong(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "emission")
				ProccesEmission(snode, material_name);
			if (node_data.name == "ambient")
				ProccesAmbient(snode, material_name);
			if (node_data.name == "diffuse")
				ProccesDiffuse(snode, material_name);
			if (node_data.name == "specular")
				ProccesSpecular(snode, material_name);
			if (node_data.name == "shininess")
				ProccesShininess(snode, material_name);
			if (node_data.name == "index_of_refraction")
				ProccesIndexOfRefraction(snode, material_name);
		}

	}
	void ColladaDae::ProccesAmbient(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "color")
				materialMap[material_name].ambient = Utils::Vec4ToVec3(Get::Vector4d(node_data.value, ' '));
			if (node_data.name == "texture")
			{
				auto texture_name = snode->first_attribute("texture");
				if (texture_name != 0)
				{
					auto att_data = Utils::GetRapidAttributeData(texture_name);
					auto name = GetName(att_data.value, "-sampler");
					materialMap[material_name].ambientTexture = texturesMap[name];
				}
			}				
		}
	}
	void ColladaDae::ProccesDiffuse(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "color")
				materialMap[material_name].diffuse = Utils::Vec4ToVec3(Get::Vector4d(node_data.value, ' '));
			if (node_data.name == "texture")
			{
				auto texture_name = snode->first_attribute("texture");
				if (texture_name != 0)
				{
					auto att_data = Utils::GetRapidAttributeData(texture_name);
					auto name = GetName(att_data.value, "-sampler");
					materialMap[material_name].diffuseTexture = texturesMap[name];
				}
			}
		}
	}
	void ColladaDae::ProccesSpecular(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "color")
				materialMap[material_name].specular = Utils::Vec4ToVec3(Get::Vector4d(node_data.value, ' '));
			if (node_data.name == "texture")
			{
				auto texture_name = snode->first_attribute("texture");
				if (texture_name != 0)
				{
					auto att_data = Utils::GetRapidAttributeData(texture_name);
					auto name = GetName(att_data.value, "-sampler");
					materialMap[material_name].specularTexture = texturesMap[name];
				}
			}
		}
	}
	void ColladaDae::ProccesShininess(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "float")
				materialMap[material_name].shineDamper = Utils::StringToFloat(node_data.value);			
		}
	}
	void ColladaDae::ProccesIndexOfRefraction(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "float")
				materialMap[material_name].indexOfRefraction = Utils::StringToFloat(node_data.value);
		}
	}
	void ColladaDae::ProccesEmission(XMLNode * root, const std::string& material_name)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "color")
			{
				//To do... ?
			}
		}
	}
	void ColladaDae::ProccesVisualScebeNode(XMLNode* root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "node")
				ProccesVisualSceneNodeInNode(snode);
		}
	}
	void ColladaDae::ProccesVisualSceneNodeInNode(XMLNode* root)
	{
		auto id_attribute = root->first_attribute("id");

		if (id_attribute == 0)
		{
			Log(parsingFileName + " : ColladaDae::ProccesVisualScebeNode attribute 'id' not found.");
			return;
		}
		auto att_data = Utils::GetRapidAttributeData(id_attribute);
		auto obj = std::find_if(objects.begin(), objects.end(), [&att_data](WBLoader::Object o)
		{
			return (o.name == att_data.value);

		});

		if (obj == objects.end())
		{
			Log(parsingFileName + " : ColladaDae::ProccesVisualScebeNode object '" + att_data.value + "' not found.");
			return;
		}

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "matrix")
			{
				obj->transformMatrix = GetMatrixFromString(node_data.value);
				//Hack, object is rotated
				obj->transformMatrix*= glm::rotate(-90.f, 1.0f, 0.0f, 0.0f);
			}
		}
	}
	void ColladaDae::ProccesVisualSceneNodeInNode(XMLNode* root, WBLoader::Object& object)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "matrix")
			{
			}
		}
	}
	void ColladaDae::ProccesController(XMLNode * root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "skin")
				ProccesSkin(snode);
		}
	}
	void ColladaDae::ProccesSkin(XMLNode * root)
	{
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "source")
				ProccesSkinSource(snode);
		}
	}
	void ColladaDae::ProccesSkinSource(XMLNode * root)
	{
		Mat4Vector jointsMatrixes;
		StrVector  joinNames;
		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "Name_array")
				joinNames = ProccesNameArray(snode);
			if (node_data.name == "float_array")
			{
				auto id = snode->first_attribute("id");
				auto att_data = Utils::GetRapidAttributeData(id);
				auto type = GetArrayType(att_data.value);

				if (type == ColladaTypes::BIND_POSES_ARRAY)
				{
					jointsMatrixes = GetMatrixesFromString(node_data.value);
				}
				else if (type == ColladaTypes::JOINT_WEIGHTS)
				{

				}
			}
		}
	}
	StrVector ColladaDae::ProccesNameArray(XMLNode * root)
	{
		StrVector joinNames;
		auto id_attribute = root->first_attribute("id");

		if (id_attribute == 0)
		{
			Log(parsingFileName + "::" + "attribute 'id' dosent exist :: void ColladaDae::ProccesNameArray(XMLNode * root)");
			return joinNames;
		}

		auto att_data = Utils::GetRapidAttributeData(id_attribute);
		auto arrayType = GetArrayType(att_data.value);

		if (arrayType == ColladaTypes::JOINT_NAMES)
		{
			auto node_data = Utils::GetRapidNodeData(root);
			joinNames = GetStringsFromString(node_data.value);
			Log("Joins in " + parsingFileName);
			for(const auto& join : joinNames)
				Log("\t -" + join);
		}
		return joinNames;
	}
	void ColladaDae::ProccesMeshSourceNode(XMLNode* root, WBLoader::Mesh& out_mesh)
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
	void ColladaDae::ProccesPolyListNode(XMLNode* root, WBLoader::Mesh& out_mesh)
	{
		auto id_attribute = root->first_attribute("material");
		if (id_attribute != 0)
		{
			auto att_data = Utils::GetRapidAttributeData(id_attribute);
			auto material_name = GetName(att_data.value, "-material");

			out_mesh.material = materialMap[material_name];
		}

		int buffers_count = 0;

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);
			if (node_data.name == "input")
				++buffers_count;
		}

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "p")
			{
				auto indices = GetIntsFromString(node_data.value);

				for (uint32 x = 0; x < indices.size(); x += buffers_count)
				{
					WBLoader::VertexBuffer vb;

					if(buffers_count >= 1)
					vb.position = out_mesh.vertex[indices[x]];
					if (buffers_count >= 2)
					vb.normal = out_mesh.normals[indices[x + 1]];
					if (buffers_count >= 3)
					vb.uvs = out_mesh.text_coords[indices[x + 2]];

					if (buffers_count >= 1)
					vb.indexes.x = indices[x];
					if (buffers_count >= 2)
					vb.indexes.y = indices[x + 1];
					if (buffers_count >= 3)
					vb.indexes.z = indices[x + 2];

					out_mesh.vertexBuffer.push_back(vb);
				}
			}
		}
	}
	WBLoader::Object ColladaDae::ProccesGeometryNode(XMLNode* root)
	{
		WBLoader::Object out;

		auto id_attribute = root->first_attribute("id");

		if (id_attribute != 0)
		{
			auto att_data = Utils::GetRapidAttributeData(id_attribute);
			out.name = GetName(att_data.value, "-mesh");
		}

		for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
		{
			auto node_data = Utils::GetRapidNodeData(snode);

			if (node_data.name == "mesh")
				out.meshes.push_back(ProccesMeshNode(snode));
		}

		return out;
	}
	WBLoader::Mesh ColladaDae::ProccesMeshNode(XMLNode* root)
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
	mat4 ColladaDae::GetMatrixFromString(const std::string& str) const
	{
		auto mdata = GetFloatsFromString(str);
		mat4 transform_matrix(1.f);
		if (mdata.size() != 16)
		{
			Log("ColladaDae::GetMatrixFromString string is wrong size.");
			return transform_matrix;
		}
		
		uint16 i = 0;
		for (uint16 y = 0; y < 4; ++y)
			for (uint16 x = 0; x < 4; ++x)
				transform_matrix[x][y] = mdata[i++];

		return transform_matrix;
	}
	Mat4Vector ColladaDae::GetMatrixesFromString(const std::string & str) const
	{
		Mat4Vector output;
		auto mdata = GetFloatsFromString(str);

		mat4 transform_matrix(1.f);
		int y = 0, x = 0;
		for (const auto& f : mdata)
		{
			transform_matrix[x][y] = f;
			++x;
			if (x >= 4)
			{
				x = 0;
				++y;
			}

			if (y >= 4)
			{
				output.push_back(transform_matrix);
				y = 0;
				x = 0;
			}
		}

		return output;
	}
	Vec2Vector ColladaDae::GetVectors2dFromString(const std::string& str) const
	{
		Vec2Vector out;

		auto strs = Utils::SplitString(str, ' ');

		FloatVector tmp;

		for (const auto& str : strs)
		{
			auto a = Utils::StringToFloat(str);
			tmp.push_back(a);

			if (tmp.size() >= 2)
			{
				vec2 v(tmp[0], tmp[1]);
				out.push_back(v);
				tmp.clear();
			}
		}

		return out;
	}
	Vec3Vector ColladaDae::GetVectors3dFromString(const std::string& str) const
	{
		Vec3Vector out;

		auto strs = Utils::SplitString(str, ' ');

		FloatVector tmp;

		for (const auto& str : strs)
		{
			auto a = Utils::StringToFloat(str);
			tmp.push_back(a);

			if (tmp.size() >= 3)
			{
				vec3 v(tmp[0], tmp[1], tmp[2]);
				out.push_back(v);
				tmp.clear();
			}
		}

		return out;
	}
	FloatVector ColladaDae::GetFloatsFromString(const std::string& str) const
	{
		FloatVector out;

		auto strs = Utils::SplitString(str, ' ');

		for (const auto& str : strs)
		{
			auto a = Utils::StringToFloat(str);
			out.push_back(a);
		}

		return out;
	}
	Uint16Vector ColladaDae::GetIntsFromString(const std::string& str) const
	{
		Uint16Vector out;

		auto strs = Utils::SplitString(str, ' ');

		for (const auto& str : strs)
		{
			auto a = Utils::StringToInt(str);
			out.push_back(a);
		}
		return out;
	}
	StrVector ColladaDae::GetStringsFromString(const std::string & str) const
	{
		return Utils::SplitString(str, ' ');
	}
	std::string ColladaDae::GetName(const std::string& str, const std::string& postfix) const
	{
		return str.substr(0, str.find(postfix));
	}
	ColladaTypes::ArrayType ColladaDae::GetArrayType(const std::string& str) const
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

		auto join_names = str.find("-joints-array");
		if (join_names != std::string::npos)
			return ColladaTypes::ArrayType::JOINT_NAMES;

		auto join_weights = str.find("-weights-array");
		if (join_weights != std::string::npos)
			return ColladaTypes::ArrayType::JOINT_WEIGHTS;

		auto bind_poses = str.find("-bind_poses-array");
		if (bind_poses != std::string::npos)
			return ColladaTypes::ArrayType::BIND_POSES_ARRAY;

		return ColladaTypes::ArrayType::UNKNOWN;
	}
}