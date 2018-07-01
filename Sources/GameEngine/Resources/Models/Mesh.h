#pragma once
#include "Utils.h"
#include "Types.h"
#include "Material.h"
#include "MeshRawData.h"
#include "BoundingBox.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include <vector>

class CMesh : public COpenGLObject
{
public:
    CMesh(GameEngine::IGraphicsApiPtr graphicsApi);
	CMesh(GameEngine::IGraphicsApiPtr graphicsApi, const SMaterial& material, const mat4& transformMatix = mat4(1.f));
    CMesh(const CMesh&)         = delete;
    CMesh(CMesh&&)              = default;
    virtual ~CMesh();
    virtual void OpenGLLoadingPass() override;
    virtual void OpenGLPostLoadingPass() override;
    void SetMaterial(const SMaterial& material);
	void SetTransformMatrix(const glm::mat4& m);
    bool IsInit() const;
	bool UseArmature() const;
    const BoundingBox& GetBoundingBox() const;

    uint32 GetObjectId() const;
    const SMaterial& GetMaterial() const;
	inline const mat4& GetMeshTransform() const;
	inline GameEngine::MeshRawData& GetMeshDataRef();
	inline const GameEngine::MeshRawData& GetCMeshDataRef() const;

private:
    void CreateMesh();
    void CalculateBoudnigBox(const std::vector<float>& positions);
    void SetInstancedMatrixes(const std::vector<mat4>& m);

    void ClearData();

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;
	GameEngine::MeshRawData meshRawData_;
    SMaterial material_;
	uint32 objectId_;

    bool isInit              = false;
    bool transformVboCreated = false;
    bool useAramture         = false;

    // local transform in mesh
    mat4 transform_;
    BoundingBox boundingBox;
};

const mat4& CMesh::GetMeshTransform() const
{
	return transform_;
}

GameEngine::MeshRawData& CMesh::GetMeshDataRef()
{
	return meshRawData_;
}

inline const GameEngine::MeshRawData & CMesh::GetCMeshDataRef() const
{
	return meshRawData_;
}
