#pragma once
#include "../Types/Effects/LibraryEffects.h"
#include "ColladaCommon.h"

namespace GameEngine
{
namespace Collada
{
class LibraryEffectsReader
{
public:
    LibraryEffectsReader(LibraryEffects& libMaterial)
        : data_(libMaterial)
    {
    }
    void read(XMLNode* node);

private:
    void ProcessEffect(XMLNode* node);
    void ProcessFloat(XMLNode* node, Float& f);
    void ProcessColor(XMLNode* node, Color& f);

private:
    LibraryEffects& data_;
};
}  // Collada
}  // GameEngine
