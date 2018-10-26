#include "GeometryReader.h"
#include <rapidxml.hpp>
#include "../Types/Common/Source.h"
#include "Logger/Log.h"
#include "Utils.h"
#include "XML/XMLUtils.h"

namespace GameEngine
{
namespace Collada
{
void LibraryGeometriesReader::readGeometries(XMLNode* node)
{
    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "geometry")
        {
            ProcessGeometry(snode);
        }
    });
}
void LibraryGeometriesReader::ProcessGeometry(XMLNode* node)
{
    auto aid = Utils::GetRapidAttributeData(node, "id");

    if (aid.value.empty())
        return;

    geometyData_.geometries_[aid.value].id_   = aid.value;
    geometyData_.geometries_[aid.value].name_ = Utils::GetRapidAttributeData(node, "name").value;
    geometyData_.geometries_[aid.value].meshes_.emplace_back();

    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "mesh")
        {
            ProcessMesh(snode, geometyData_.geometries_[aid.value].meshes_.back());
        }
    });
}
void LibraryGeometriesReader::ProcessMesh(XMLNode* node, Mesh& mesh)
{
    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "source")
        {
            auto aId = Utils::GetRapidAttributeData(snode, "id");
            if (aId.value.empty())
                return;

            ProcessSource(snode, mesh.sources_[aId.value]);
        }
        else if (data.name == "vertices")
        {
            auto aId = Utils::GetRapidAttributeData(snode, "id");
            if (aId.value.empty())
                return;
            ProcessVertices(snode, mesh.vertices_[aId.value]);
        }
        else if (data.name == "polylist")
        {
            auto smaterial = Utils::GetRapidAttributeData(snode, "material").value;
            if (smaterial.empty())
                return;

            mesh.polylist_[smaterial].materialId_ = smaterial;
            ProcessPolyList(snode, mesh.polylist_[smaterial]);
        }
        else if (data.name == "triangles")  // To do :  create vcount with 3 3 3 3 ...
        {
            auto smaterial = Utils::GetRapidAttributeData(snode, "material").value;
            if (smaterial.empty())
                return;
            auto scount = Utils::GetRapidAttributeData(snode, "count").value;
            if (!scount.empty())

                mesh.polylist_[smaterial].materialId_ = smaterial;
            mesh.polylist_[smaterial].vcount_         = std::vector<uint32>(std::stoi(scount), 3);
            ProcessPolyList(snode, mesh.polylist_[smaterial]);
        }
    });
}
void LibraryGeometriesReader::ProcessVertices(XMLNode* node, Vertices& vert)
{
    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (data.name == "input")
        {
            vert.inputs_.emplace_back();
            ProcessInput(snode, vert.inputs_.back());
        }
    });
}
void LibraryGeometriesReader::ProcessPolyList(XMLNode* node, Polylist& polylist)
{
    auto scount = Utils::GetRapidAttributeData(node, "count").value;

    Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode) {
        if (!scount.empty())
            polylist.count_ = std::stoi(scount);

        if (data.name == "input")
        {
            polylist.inputs_.emplace_back();
            ProcessInput(snode, polylist.inputs_.back());
        }
        else if (data.name == "vcount")
        {
            polylist.vcount_ = GetIntsFromString(data.value);
        }
        else if (data.name == "p")
        {
            polylist.p_ = GetIntsFromString(data.value);
        }
    });
}
}  // Collada
}  // GameEngine
