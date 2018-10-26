#include "libraryEffectsReader.h"
#include <rapidxml.hpp>
#include "../Types/Common/Source.h"
#include "Logger/Log.h"
#include "Utils.h"
#include "XML/XMLUtils.h"

namespace GameEngine
{
namespace Collada
{
void LibraryEffectsReader::read(XMLNode* node)
{
    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "effect")
        {
            ProcessEffect(snode);
        }
    });
}
void LibraryEffectsReader::ProcessEffect(XMLNode* node)
{
    auto aid = Utils::GetRapidAttributeData(node, "id");

    if (aid.value.empty())
        return;

    data_.effects_[aid.value].id = aid.value;

    auto pc = node->first_node("profile_COMMON");

    if (pc == 0)
        return;

    Utils::ForEachSubNode(pc, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "newparam")
        {
            auto pId       = Utils::GetRapidAttributeData(snode, "sid").value;
            auto& newParam = data_.effects_[aid.value].newParams_[pId];
            newParam.id_   = pId;

            Utils::ForEachSubNode(snode, [&](const Utils::RapidNodeData& newParamData, XMLNode* newnode) {
                if (newParamData.name == "surface")
                {
                    auto initFromNode = newnode->first_node("init_from");
                    if (initFromNode == 0)
                        return;

                    newParam.surface_ = {Utils::GetRapidAttributeData(newnode, "type").value,
                                         Utils::GetRapidNodeData(initFromNode).value};
                }
                else if (newParamData.name == "sampler2D")
                {
                    auto sourceNode = newnode->first_node("source");
                    if (sourceNode == 0)
                        return;
                    newParam.sampler_ = {"2D", Utils::GetRapidNodeData(sourceNode).value};
                }
            });
        }
        else if (data.name == "technique")
        {
            auto phong = snode->first_node("phong");
            if (phong == 0)
                return;

            Utils::ForEachSubNode(phong, [&](const Utils::RapidNodeData& phongData, XMLNode* pnode) {
                if (phongData.name == "emission")
                {
                    ProcessColor(pnode, data_.effects_[aid.value].emission);
                }
                if (phongData.name == "ambient")
                {
                    ProcessColor(pnode, data_.effects_[aid.value].ambient);
                }
                if (phongData.name == "diffuse")
                {
                    ProcessColor(pnode, data_.effects_[aid.value].diffuse);
                }
                if (phongData.name == "specular")
                {
                    ProcessColor(pnode, data_.effects_[aid.value].specular);
                }
                if (phongData.name == "shininess")
                {
                    ProcessFloat(pnode, data_.effects_[aid.value].shininess);
                }
                if (phongData.name == "index_of_refraction")
                {
                    ProcessFloat(pnode, data_.effects_[aid.value].indexOfRefraction);
                }
            });
        }
    });
}
void LibraryEffectsReader::ProcessFloat(XMLNode* node, Float& f)
{
    auto floatnode = node->first_node("float");
    if (floatnode == 0)
        return;

    auto fdata = Utils::GetRapidNodeData(floatnode);
    auto aid   = Utils::GetRapidAttributeData(floatnode, "sid");

    f.sid   = aid.value;
    f.value = std::stof(fdata.value);
}
void LibraryEffectsReader::ProcessColor(XMLNode* node, Color& f)
{
    auto colornode = node->first_node("color");
    if (colornode != 0)
    {
        auto colorData = Utils::GetRapidNodeData(colornode);
        f.sid          = Utils::GetRapidAttributeData(colornode, "sid").value;
        f.value        = GetVector4FromString(colorData.value);
    }

    auto texturenode = node->first_node("texture");
    if (texturenode != 0)
    {
        f.texture  = Utils::GetRapidAttributeData(texturenode, "texture").value;
        f.texcoord = Utils::GetRapidAttributeData(texturenode, "texcoord").value;
    }
}
}  // Collada
}  // GameEngine
