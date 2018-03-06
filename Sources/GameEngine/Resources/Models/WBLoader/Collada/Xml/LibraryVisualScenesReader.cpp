#include "LibraryVisualScenesReader.h"
#include "../Types/Common/Source.h"
#include "XML/XMLUtils.h"
#include "Logger/Log.h"
#include "Utils.h"
#include <rapidxml.hpp>

namespace GameEngine
{
	namespace Collada
	{
		void LibraryVisualSceneReader::read(XMLNode* node)
		{
			Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode)
			{
				if (data.name == "visual_scene")
				{
					ProccesVisualScene(snode);
				}
			});
		}
		void LibraryVisualSceneReader::ProccesVisualScene(XMLNode * node)
		{
			auto id = Utils::GetRapidAttributeData(node, "id").value;
			if (id.empty())
				return;

			auto& vs = data_.visualScenes_[id];
			vs.id_ = id;
			vs.name_ = Utils::GetRapidAttributeData(node, "name").value;

			Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode)
			{
				if (data.name == "node")
				{
					auto nodeid = Utils::GetRapidAttributeData(snode, "id").value;
					if (nodeid.empty())
						return;
					
					auto& newNode = vs.nodes_[nodeid];
					newNode.id_ = nodeid;
					newNode.name_ = Utils::GetRapidAttributeData(snode, "name").value;
					newNode.type_ = Utils::GetRapidAttributeData(snode, "type").value;
					ProccesNode(snode, newNode);
				}
			});
		}
		void LibraryVisualSceneReader::ProccesNode(XMLNode* xmlnode, Node& node)
		{
			Utils::ForEachSubNode(xmlnode, [&](const Utils::RapidNodeData& data, XMLNode* snode)
			{
				if (data.name == "translate")
				{
					auto v = GetVectors3dFromString(data.value);
					if (v.empty())
						return;

					node.translate_ = {v.front(), Utils::GetRapidAttributeData(snode, "sid").value};
				}
				else if (data.name == "rotate")
				{
					auto v = GetVectors3dFromString(data.value);
					if (v.empty())
						return;

					node.rotate_ = { v.front(), Utils::GetRapidAttributeData(snode, "sid").value };
				}
				else if (data.name == "scale")
				{
					auto v = GetVectors3dFromString(data.value);
					if (v.empty())
						return;

					node.scale_ = { v.front(), Utils::GetRapidAttributeData(snode, "sid").value };
				}
				else if (data.name == "matrix")
				{
					if (data.value.empty())
						return;

					auto m = GetMatrixFromString(data.value);
					node.matrix_ = { m, Utils::GetRapidAttributeData(snode, "sid").value };
				}
				else if (data.name == "instance_controller")
				{
					node.instanceController_ = InstanceController();
					ProccesInstanceController(snode, node.instanceController_.value());
				}
				else if (data.name == "node")
				{
					auto nodeid = Utils::GetRapidAttributeData(snode, "id").value;
					if (nodeid.empty())
						return;

					node.children_[nodeid] = std::make_shared<Node>();
					auto& child = *node.children_[nodeid].get();
					child.id_ = nodeid;
					child.sid_ = Utils::GetRapidAttributeData(snode, "sid").value;
					child.name_ = Utils::GetRapidAttributeData(snode, "name").value;
					child.type_ = Utils::GetRapidAttributeData(snode, "type").value;

					ProccesNode(snode, child);
				}
			});
		}
		void LibraryVisualSceneReader::ProccesInstanceController(XMLNode* node, InstanceController & instanceController)
		{
			instanceController.url_ = Utils::GetRapidAttributeData(node, "url").value;

			Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode)
			{
				if (data.name == "skeleton")
				{
					instanceController.skeleton_ = data.value;
				}
				else if (data.name == "bind_material")
				{

				}
			});
		}
	} // Collada
} // GameEngine
