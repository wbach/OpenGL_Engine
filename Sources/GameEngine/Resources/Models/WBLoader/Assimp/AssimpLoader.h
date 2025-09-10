#pragma once
#include <Types.h>

#include <string>

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
    void reset()
    {
        id_ = 0;
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
    void processSkeleton(const aiScene&);
    void processAnimations(const aiScene&);

    Animation::AnimationClip processAnimation(const aiAnimation&);
    const aiNode* findArmatureRootNode(const aiNode&);
    void createSkeleton(const aiNode&, Animation::Joint&);
    Animation::KeyFrame& getFrameByTimeStamp(Animation::AnimationClip&, float);
    void readAdditionInfoFile(const File&);
    void printTree(const aiNode&, uint32 = 0) const;
    std::string getTexturePath(const std::string&) const;

private:
    std::optional<File> currentProcessingFile_;
    WBLoader::Object* object_;
    BonesMap bones_;
    BoneIdPool boneIdPool_;
};
}  // namespace WBLoader
}  // namespace GameEngine
