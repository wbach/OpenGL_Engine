#include "LibraryImagesReader.h"
#include "../Types/Common/Source.h"
#include "XML/XMLUtils.h"
#include "Logger/Log.h"
#include "Utils.h"
#include <rapidxml.hpp>

namespace GameEngine
{
	namespace Collada
	{
		void LibraryImagesReader::read(XMLNode* node)
		{
			Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode)
			{
				if (data.name == "image")
				{
					ProcessImages(snode);
				}
			});
		}
		void LibraryImagesReader::ProcessImages(XMLNode * node)
		{
			auto aid = Utils::GetRapidAttributeData(node, "id");

			if (aid.value.empty())
				return;

			auto& image = data_.images_[aid.value];
			image.id_ = aid.value;
			image.name_ = Utils::GetRapidAttributeData(node, "name").value;

			Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode)
			{
				if (data.name == "init_from")
				{
					image.initFrom_ = data.value;
				}
			});
		}
	} // Collada
} // GameEngine
