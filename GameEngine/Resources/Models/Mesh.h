#pragma once
#include <vector>
#include "Material.h"
#include "Bone.h"
#include "../../Utils/Utils.h"
#include "BoundingBox.h"

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
		BONES,
		COUNT,
	};
}

class CMesh : public COpenGLObject
{
public:
	CMesh();
	CMesh(const SMaterial& material, const std::vector<float>& positions, const std::vector<float>& text_coords = std::vector<float>(), const std::vector<float>& normals = std::vector<float>(), const std::vector<float>& tangents = std::vector<float>(),
		const std::vector<uint16>& indices = std::vector<uint16>(), const std::vector<SVertexBoneData>& bones = std::vector<SVertexBoneData>());
	CMesh(const CMesh&) = delete;
	CMesh(CMesh&&) = default;

	virtual ~CMesh();
	virtual void OpenGLLoadingPass() override;
	virtual void OpenGLPostLoadingPass() override;

	void CreateVaoMesh();
	void CreateTransformsVbo(std::vector<glm::mat4>& m);
	void UpdateTransformVbo(std::vector<glm::mat4>& m);
	void CreateBoneVbo(const std::vector<SVertexBoneData>& bones);
	void CalculateBoudnigBox(const std::vector<float>& positions);
	void UpdateVertexPosition(const std::vector<float>& vertices) const;
	void SetInstancedMatrixes(const std::vector<glm::mat4>& m);

	const glm::vec3& GetBoundingSize();
	const glm::vec3& GetBoundingMin();
	const glm::vec3& GetBoundingMax();
	const glm::vec3& GetBoundingCenter();

    const uint32& GetVao() const;
	const uint32& GetVbo(VertexBufferObjects::Type type) const;
	const uint32& GetVertexCount() const;
	const SMaterial& GetMaterial() const;
    const std::vector<uint32>& GetUsedAttributes() const { return attributes; }

    void SetMaterial(const SMaterial& material);

	void ClearData();

private:
    SMaterial material;

    uint32 vao;
    uint32 vbos[VertexBufferObjects::COUNT];
	uint32 vertexCount;

	//used attributes
    std::vector<uint32> attributes;

    std::vector<float> positions;
    std::vector<float> textCoords;
    std::vector<float> normals;
    std::vector<float> tangents;
	std::vector<float> bitangents;
    std::vector<uint16> indices;
    std::vector<SVertexBoneData> bones;
    std::vector<glm::mat4> instancedMatrixes;

    bool isInit = false;
    bool transformVboCreated = false;
    bool bonesInShader = false;

	BoundingBox boundingBox;
};
