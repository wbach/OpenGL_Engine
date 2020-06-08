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
    void ParseFile(const File&) override;
    bool CheckExtension(const File&) override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
};
}  // namespace WBLoader
}  // namespace GameEngine
