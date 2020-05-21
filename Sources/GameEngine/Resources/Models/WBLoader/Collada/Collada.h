#pragma once
#include "../AbstractLoader.h"
#include "../MeshData.h"
#include "Types/ColladaData.h"
#include "Xml/ColladaCommon.h"

namespace GameEngine
{
class ITextureLoader;

namespace WBLoader
{
class ColladaDae : public AbstractLoader
{
public:
    ColladaDae(ITextureLoader& textureLodaer);
    virtual void ParseFile(const File&) override;
    virtual bool CheckExtension(const std::string& filename) override;

private:
    void ConstructModel();
    void FillAnimationData();
    void NewGeometry(const Collada::Geometry&);
    void NewMesh(const Collada::Mesh&, const std::string& geometryName);
    void GetVec2ArrayFromDataSource(const Collada::Mesh& mesh, const std::string& sourceId,
                                    std::vector<vec2>& v);
    void GetVec3ArrayFromDataSource(const Collada::Mesh& mesh, const std::string& sourceId,
                                    std::vector<vec3>& v);
    void PrepareMeshData(const Collada::Mesh& colladaMesh, Mesh& newMesh,
                         const Collada::Input& input);
    Collada::DataTypes GetSourceType(const Collada::Source& source) const;
    void CreateSkeleton(const Collada::Node& node, Animation::Joint& joint,
                        const std::unordered_map<std::string, uint32>& joints);
    void FillAnimator(std::unordered_map<std::string, Animation::AnimationClip>& animator,
                      const std::unordered_map<std::string, uint32>& joints);

private:
    void Clear();
    void ApplyMaterials(Material& material, const std::string& materialId);
    Collada::ObjectType GetType(const std::string&);
    const Collada::Node& GetNode(const std::string& node_id);
    const Collada::Node& GetNode(const Collada::Node& node, const std::string& node_id);

private:
    Collada::ColladaData data_;
    std::unordered_map<std::string, Mesh*> idToMeshMap_;
};
}
}  // namespace GameEngine
