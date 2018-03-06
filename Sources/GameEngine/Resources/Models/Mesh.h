#pragma once
#include "Types.h"
#include "BoundingBox.h"
#include "Material.h"
#include "Utils.h"
#include "GameEngine/Resources/Models/Animation/Joint.h"
#include <vector>

namespace VertexBufferObjects
{
enum Type
{
    INDICES = 0,
    POSITION,
    TEXT_COORD,
    NORMAL,
    TANGENT,
    TRANSFORM_MATRIX,
    JOINTS,
	WEIGHTS,
    COUNT,
};
}

typedef std::vector<float> FloatVec;
typedef std::vector<uint16> UintVec;
typedef std::vector<uint8> Uint8Vec;

class CMesh : public COpenGLObject
{
public:
    CMesh();
	CMesh
	(
		const SMaterial& material,
		const FloatVec& positions,
		const FloatVec& text_coords = {},
		const FloatVec& normals = {},
		const FloatVec& tangents = {},
		const UintVec& indices = {},
		const UintVec& joinIds = {},
		const FloatVec& bonesWeights = {},
		const mat4& transform = mat4(1.f)
	);

    CMesh(const CMesh&)         = delete;
    CMesh(CMesh&&)              = default;

    virtual ~CMesh();

    virtual void OpenGLLoadingPass() override;
    virtual void OpenGLPostLoadingPass() override;

    bool IsInit() const;
    const BoundingBox& GetBoundingBox() const;

    const uint32& GetVao() const;
    const uint32& GetVertexCount() const;
    const SMaterial& GetMaterial() const;
	inline const mat4& GetMeshTransform() const;
	inline const Uint8Vec& GetUsedAttributes() const;

private:
    void CreateVaoMesh();
    void CreateTransformsVbo(std::vector<mat4>& m);
    void UpdateTransformVbo(std::vector<mat4>& m);
    void CalculateBoudnigBox(const std::vector<float>& positions);
    void UpdateVertexPosition(const std::vector<float>& vertices) const;
    void SetInstancedMatrixes(const std::vector<mat4>& m);

    const uint32& GetVbo(VertexBufferObjects::Type type) const;
    void SetMaterial(const SMaterial& material);
    void ClearData();

private:
    SMaterial material_;

    uint32 vao_;
    uint32 vbos_[VertexBufferObjects::COUNT];
    uint32 vertexCount_;

    // used attributes
	Uint8Vec attributes_;

	FloatVec positions_;
	FloatVec textCoords_;
	FloatVec normals_;
	FloatVec tangents_;
	FloatVec bitangents_;
	FloatVec bonesWeights_;
	UintVec indices_;
	UintVec joinIds_;
    std::vector<mat4> instancedMatrixes_;

    bool isInit              = false;
    bool transformVboCreated = false;
    bool bonesInShader       = false;

    // local transform in mesh
    mat4 transform_;
    BoundingBox boundingBox;

	GameEngine::Animation::Joint rootJoint_;
};

const Uint8Vec& CMesh::GetUsedAttributes() const
{
	return attributes_;
}

const mat4& CMesh::GetMeshTransform() const
{
	return transform_;
}