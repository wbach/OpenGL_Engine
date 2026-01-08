#pragma once
#include <list>

#include "GameEngine/Resources/Models/AbstractLoader.h"

namespace GameEngine
{
class ITextureLoader;
class BinaryLoader : public AbstractLoader
{
public:
    BinaryLoader(ITextureLoader&);
    bool ParseFile(const File&) override;
    std::unique_ptr<Model> Create() override;
    bool CheckExtension(const File&) override;

private:
    File currentFile;
};
}  // namespace GameEngine
