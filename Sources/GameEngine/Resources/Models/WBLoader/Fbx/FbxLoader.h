#pragma once
#include <Types.h>
#include "../AbstractLoader.h"
#include "../MeshData.h"

namespace GameEngine
{
namespace WBLoader
{
class FbxLoader : public AbstractLoader
{
public:
    FbxLoader(ITextureLoader&);
    ~FbxLoader();
    virtual void ParseFile(const File&) override;
    virtual bool CheckExtension(const std::string&) override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
};
}  // namespace WBLoader
}  // namespace GameEngine
