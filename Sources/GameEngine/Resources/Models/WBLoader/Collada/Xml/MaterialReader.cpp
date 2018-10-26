#include "MaterialReader.h"
#include <rapidxml.hpp>
#include "../Types/Common/Source.h"
#include "Logger/Log.h"
#include "Utils.h"
#include "XML/XMLUtils.h"

namespace GameEngine
{
namespace Collada
{
void LibraryMaterialReader::read(XMLNode* node)
{
    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "material")
        {
            ProcessMaterial(snode);
        }
    });
}
void LibraryMaterialReader::ProcessMaterial(XMLNode* node)
{
    auto aid = Utils::GetRapidAttributeData(node, "id");

    if (aid.value.empty())
        return;

    data_.materials_[aid.value].id = aid.value;

    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "instance_effect")
        {
            data_.materials_[aid.value].instanceEffect = Utils::GetRapidAttributeData(snode, "url").value;
        }
    });
}
}  // Collada
}  // GameEngine
