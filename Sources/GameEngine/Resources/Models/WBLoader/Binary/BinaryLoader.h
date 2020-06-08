#pragma once
#include <list>
#include "../AbstractLoader.h"

namespace GameEngine
{
class TextureLoader;

namespace WBLoader
{
class BinaryLoader : public AbstractLoader
{
public:
    BinaryLoader(TextureLoader& textureLodaer);
    void ParseFile(const File&) override;
    bool CheckExtension(const File&) override;
};
}
}  // namespace GameEngine
