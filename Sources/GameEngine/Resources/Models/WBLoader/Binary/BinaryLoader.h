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
    virtual void ParseFile(const std::string& filename) override;
    virtual bool CheckExtension(const std::string& filename) override;
};
}
}  // namespace GameEngine
