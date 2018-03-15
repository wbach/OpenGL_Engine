#pragma once
#include "Types.h"
#include "BoundingBox.h"
#include "Material.h"
#include "Utils.h"
#include "MeshRawData.h"
#include "GameEngine/Resources/VboTypes.h"
#include <vector>



class CMesh : public COpenGLObject
{
public:
    CMesh();
	CMesh(const SMaterial& material, const mat4& transformMatix = mat4(1.f));
    CMesh(const CMesh&)         = delete;
    CMesh(CMesh&&)              = default;
    virtual ~CMesh();
    virtual void OpenGLLoadingPass() override;
    virtual void OpenGLPostLoadingPass() override;

    bool IsInit() const;
	bool UseArmature() const;
    const BoundingBox& GetBoundingBox() const;

    const uint32& GetVao() const;
    const uint32& GetVertexCount() const;
    const SMaterial& GetMaterial() const;
	inline const mat4& GetMeshTransform() const;
	inline const VboMap& GetUsedAttributes() const;

	inline GameEngine::MeshRawData& GetMeshDataRef();

private:
    void CreateVaoMesh();
    void CreateTransformsVbo(std::vector<mat4>& m);
    void UpdateTransformVbo(std::vector<mat4>& m);
    void CalculateBoudnigBox(const std::vector<float>& positions);
    void UpdateVertexPosition(const std::vector<float>& vertices) const;
    void SetInstancedMatrixes(const std::vector<mat4>& m);

    uint32 GetVbo(VertexBufferObjects type) const;
    void SetMaterial(const SMaterial& material);
    void ClearData();

private:
    SMaterial material_;

    uint32 vao_;
	VboMap vbos_;
    uint32 vertexCount_;

    // used attributes
	VboMap attributes_;
	GameEngine::MeshRawData meshRawData_;

    bool isInit              = false;
    bool transformVboCreated = false;
    bool useAramture         = false;

    // local transform in mesh
    mat4 transform_;
    BoundingBox boundingBox;
};

const VboMap& CMesh::GetUsedAttributes() const
{
	return attributes_;
}

const mat4& CMesh::GetMeshTransform() const
{
	return transform_;
}

GameEngine::MeshRawData& CMesh::GetMeshDataRef()
{
	return meshRawData_;
}
