#pragma once
#include "../Types/Materials/LibraryMaterials.h"
#include "ColladaCommon.h"

namespace GameEngine
{
namespace Collada
{
class LibraryMaterialReader
{
public:
    LibraryMaterialReader(LibraryMaterials& libMaterial)
        : data_(libMaterial)
    {
    }
    void read(XMLNode* node);

private:
    void ProcessMaterial(XMLNode* node);

private:
    LibraryMaterials& data_;
};
}  // Collada
}  // GameEngine
