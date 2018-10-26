#include "LibraryControllersReader.h"
#include <rapidxml.hpp>
#include "../Types/Common/Source.h"
#include "Logger/Log.h"
#include "Utils.h"
#include "XML/XMLUtils.h"

namespace GameEngine
{
namespace Collada
{
void LibraryControllersReader::read(XMLNode* node)
{
    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "controller")
        {
            Process(snode);
        }
    });
}
void LibraryControllersReader::Process(XMLNode* node)
{
    auto controllerId = Utils::GetRapidAttributeData(node, "id").value;

    if (controllerId.empty())
        return;

    auto& controller = data_.controllers_[controllerId];
    controller.id_   = controllerId;
    controller.name_ = Utils::GetRapidAttributeData(node, "name").value;

    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "skin")
        {
            auto skinSource                      = Utils::GetRapidAttributeData(snode, "source").value;
            controller.skins_[skinSource].source = skinSource;
            Process(snode, controller.skins_[skinSource]);
        }
    });
}
void LibraryControllersReader::Process(XMLNode* node, Skin& skin)
{
    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "bind_shape_matrix")
        {
            auto bind_shape_matrix = Utils::GetRapidNodeData(snode).value;
            if (!bind_shape_matrix.empty())
                skin.bindShapeMatrix_ = GetMatrixFromString(bind_shape_matrix);
        }
        else if (data.name == "source")
        {
            auto sourceId = Utils::GetRapidAttributeData(snode, "id").value;
            if (sourceId.empty())
                return;

            ProcessSource(snode, skin.sources_[sourceId]);
        }
        else if (data.name == "joints")
        {
            Utils::ForEachSubNode(snode, [&](const Utils::RapidNodeData& idata, XMLNode* inputnode) {
                if (idata.name == "input")
                {
                    skin.joitns_.inputs_.emplace_back();
                    ProcessInput(inputnode, skin.joitns_.inputs_.back());
                }
            });
        }
        else if (data.name == "vertex_weights")
        {
            Process(snode, skin.vertexWeights_);
        }
    });
}

void LibraryControllersReader::Process(XMLNode* node, VertexWeights& vw)
{
    auto scount = Utils::GetRapidAttributeData(node, "count").value;
    if (!scount.empty())
        vw.count = std::stoi(scount);

    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "input")
        {
            vw.inputs_.emplace_back();
            ProcessInput(snode, vw.inputs_.back());
        }
        else if (data.name == "vcount")
        {
            vw.vcount_ = GetIntsFromString(data.value);
        }
        else if (data.name == "v")
        {
            vw.v_ = GetIntsFromString(data.value);
        }
    });
}

}  // Collada
}  // GameEngine
