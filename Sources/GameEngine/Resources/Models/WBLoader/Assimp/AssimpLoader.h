#pragma once
#include <Types.h>

#include "../AbstractLoader.h"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiNodeAnim;
struct aiAnimation;

namespace GameEngine
{
class ITextureLoader;
namespace
{
struct KeyFrame;
}
namespace WBLoader
{
class BoneIdPool
{
public:
    uint32 nextId() 
    {
        return id_++;
    }

private:
    uint32 id_{0};
};
class AssimpLoader : public AbstractLoader
{
    struct BoneInfo
    {
        mat4 matrix{mat4(1.f)};
        uint32 id{0};
    };
    using BonesMap = std::unordered_map<std::string, BoneInfo>;

public:
    AssimpLoader(ITextureLoader&);
    ~AssimpLoader() override;
    void ParseFile(const File&) override;
    bool CheckExtension(const File&) override;

private:
    void recursiveProcess(const aiScene&, const aiNode&);
    void processMesh(const aiScene&, const aiMesh&);
    void loadVertexData(const aiMesh&, GraphicsApi::MeshRawData&);
    void loadBonesData(const aiScene&, const aiMesh&, Mesh&);
    Material processMaterial(const aiScene&, const aiMesh&) const;
    void processSkeleton(const aiScene&, Mesh&, const BonesMap&, BoneIdPool&);
    void processAnimations(const aiScene&);

    Animation::AnimationClip processAnimation(const aiAnimation&);
    const aiNode* findArmatureRootNode(const aiNode&, const BonesMap&);
    void createSkeleton(const aiNode&, Animation::Joint&, const BonesMap&, BoneIdPool&);
    Animation::KeyFrame& getFrameByTimeStamp(Animation::AnimationClip&, float);
};
}  // namespace WBLoader
}  // namespace GameEngine
