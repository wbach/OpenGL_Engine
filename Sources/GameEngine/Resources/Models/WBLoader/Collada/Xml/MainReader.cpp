#include "MainReader.h"
#include <rapidxml.hpp>
#include "GameEngine/Engine/Configuration.h"
#include "GeometryReader.h"
#include "LibraryAnimationsReader.h"
#include "LibraryControllersReader.h"
#include "LibraryImagesReader.h"
#include "LibraryVisualScenesReader.h"
#include "Logger/Log.h"
#include "MaterialReader.h"
#include "Utils.h"
#include "XML/XMLUtils.h"
#include "libraryEffectsReader.h"

namespace GameEngine
{
namespace Collada
{
void ReadCollada(const std::string& filename, ColladaData& colladaData)
{
    auto fullFilePath = EngineConf_GetFullDataPathAddToRequierd(filename);
    auto fileData     = Utils::ReadFile(fullFilePath);

    if (fileData.empty())
        return;

    rapidxml::xml_document<> document;

    try
    {
        document.parse<0>(const_cast<char*>(fileData.c_str()));
    }
    catch (...)
    {
        Error("Can not parse file " + filename);
        return;
    }
    auto root = document.first_node();
    Utils::ForEachSubNode(root, [&](const Utils::RapidNodeData& data, XMLNode* node) {
        if (data.name == "library_geometries")
        {
            LibraryGeometriesReader(colladaData.libraryGeometries_).readGeometries(node);
        }
        else if (data.name == "library_materials")
        {
            LibraryMaterialReader(colladaData.libraryMaterials_).read(node);
        }
        else if (data.name == "library_effects")
        {
            LibraryEffectsReader(colladaData.libraryEffects_).read(node);
        }
        else if (data.name == "library_images")
        {
            LibraryImagesReader(colladaData.libraryImages_).read(node);
        }
        else if (data.name == "library_animations")
        {
            LibraryAnimationsReader(colladaData.libraryAnimations_).read(node);
        }
        else if (data.name == "library_controllers")
        {
            LibraryControllersReader(colladaData.libraryControllers_).read(node);
        }
        else if (data.name == "library_visual_scenes")
        {
            LibraryVisualSceneReader(colladaData.libraryVisualScenes_).read(node);
        }
    });
}
}  // Collada
}  // GameEngine
