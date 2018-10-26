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

void BinaryLoader::ParseFile(const std::string& filename)
{
}

bool BinaryLoader::CheckExtension(const std::string& filename)
{
    auto ext = Utils::GetFileExtension(filename);
    return ext == "bin" || ext == "BIN" || ext == "Bin";
}

}  // WBLoader
}  // namespace GameEngine
