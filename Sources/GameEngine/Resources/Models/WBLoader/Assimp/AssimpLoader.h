#pragma once
#include <Types.h>

#include "../AbstractLoader.h"

struct aiNode;
struct aiMesh;
struct aiScene;

namespace GameEngine
{
class ITextureLoader;

namespace WBLoader
{
class AssimpLoader : public AbstractLoader
{
public:
    AssimpLoader(ITextureLoader&);
    ~AssimpLoader() override;
    void ParseFile(const File&) override;
    bool CheckExtension(const File&) override;

private:
    void RecursiveProcess(const aiNode&, const aiScene&);
    void ProcessMesh(const aiMesh&, const aiScene&);
};
}  // namespace WBLoader
}  // namespace GameEngine
