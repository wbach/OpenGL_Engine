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

void BinaryLoader::ParseFile(const File&)
{
}

bool BinaryLoader::CheckExtension(const File& file)
{
    return file.IsFormat("bin");
}

}  // WBLoader
}  // namespace GameEngine
