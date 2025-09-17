#pragma once
#include <list>
#include <memory>

#include "../AbstractLoader.h"

namespace GameEngine
{
class ITextureLoader;
class Model;

namespace WBLoader
{
class BEngineLoader : public AbstractLoader
{
public:
    BEngineLoader(ITextureLoader&);
    ~BEngineLoader();
    bool ParseFile(const File&) override;
    bool CheckExtension(const File&) override;
    std::unique_ptr<Model> Create() override;

private:
    std::unique_ptr<Model> model_;
};
}  // namespace WBLoader
}  // namespace GameEngine
