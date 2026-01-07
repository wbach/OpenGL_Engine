#include "BinaryLoader.h"

#include "GameEngine/Resources/TextureLoader.h"

namespace GameEngine
{
BinaryLoader::BinaryLoader(ITextureLoader& textureLodaer)
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
}  // namespace GameEngine
