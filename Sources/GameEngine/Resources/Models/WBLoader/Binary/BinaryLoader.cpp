#include "BinaryLoader.h"

#include "BinaryReader.h"
#include "GameEngine/Resources/TextureLoader.h"

namespace GameEngine
{
namespace WBLoader
{
BinaryLoader::BinaryLoader(TextureLoader& textureLodaer)
    : AbstractLoader(textureLodaer.GetGraphicsApi(), textureLodaer)
{
}

bool BinaryLoader::ParseFile(const File&)
{
    return false;
}

bool BinaryLoader::CheckExtension(const File& file)
{
    return file.IsFormat("bin");
}

}  // namespace WBLoader
}  // namespace GameEngine
