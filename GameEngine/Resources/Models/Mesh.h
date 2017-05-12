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
};

class CMesh : public COpenGLObject
{
public:
	CMesh();
	CMesh(std::vector<float>& positions, std::vector<float>& text_coords, std::vector<float>& normals, std::vector<float>& tangents,
		std::vector<unsigned short>& indices, SMaterial& material, std::vector<SVertexBoneData>& bones);
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

    const uint& GetVao() const;
	const uint& GetVbo(VertexBufferObjects::Type type) const;
	const uint& GetVertexCount() const;
	const SMaterial& GetMaterial() const;
	const std::vector<uint>& GetUsedAttributes() const { return m_Attributes; }

	void SetMaterial(const SMaterial& material)
	{
		m_Material = material;
	}

	void ClearData();
private:
    SMaterial	 m_Material;
	uint m_Vao;
	uint m_Vbos[VertexBufferObjects::COUNT];
	uint m_VertexCount;
	//used attributes
	std::vector<uint> m_Attributes;

    std::vector<float> m_Positions;
	std::vector<float> m_TextCoords;
	std::vector<float> m_Normals;
	std::vector<float> m_Tangents;
	std::vector<unsigned short> m_Indices;
	std::vector<SVertexBoneData> m_Bones;
	std::vector<glm::mat4> m_InstancedMatrixes;

	bool m_IsInit = false;
	bool m_TransformVboCreated = false;
	bool m_BonesInShader = false;

	BoundingBox m_BoundingBox;
};
