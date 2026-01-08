#include "BinaryLoader.h"

#include "GameEngine/Resources/TextureLoader.h"
#include "BinaryImporter.h"
#include "Logger/Log.h"

namespace GameEngine
{
BinaryLoader::BinaryLoader(ITextureLoader& textureLodaer)
    : AbstractLoader(textureLodaer.GetGraphicsApi(), textureLodaer)
{
}

bool BinaryLoader::ParseFile(const File& file)
{
    LOG_DEBUG << "ParseFile: " << file;
    currentFile = file;
    return file.exist();
}

bool BinaryLoader::CheckExtension(const File& file)
{
    return file.IsFormat("bin");
}
std::unique_ptr<Model> BinaryLoader::Create()
{
    LOG_DEBUG << "Create: " << currentFile;
    return ImportModelBinary(graphicsApi_, textureLoader_, currentFile.GetAbsolutePath());
}
}  // namespace GameEngine
