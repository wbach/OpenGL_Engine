#include "Mesh.h"
#include "../../Utils/GLM/GLMUtils.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"

CMesh::CMesh()
{
}

CMesh::CMesh(
	const SMaterial & material,
	const std::vector<float>& positions, 
	const std::vector<float>& text_coords,
	const std::vector<float>& normals,
	const std::vector<float>& tangents,
	const std::vector<unsigned short>& indices,	
	const std::vector<SVertexBoneData>& bones)
{
	m_Positions  = std::move(positions);
	m_TextCoords = std::move(text_coords);
	m_Normals    = std::move(normals);
	m_Tangents	 = std::move(tangents);
	m_Indices	 = std::move(indices);
	m_Tangents	 = std::move(tangents);
	m_Bones		 = std::move(bones);
	m_Material	 = material;	
    m_VertexCount = m_Indices.size() > 0 ? m_Indices.size() : m_Positions.size() / 3;

	if (!m_Positions.empty())	m_Attributes.push_back(0);
	if (!m_TextCoords.empty())	m_Attributes.push_back(1);
	if (!m_Normals.empty())		m_Attributes.push_back(2);
	if (!m_Tangents.empty())	m_Attributes.push_back(3);

	CalculateBoudnigBox(m_Positions);
}

CMesh::~CMesh()
{
	if (!m_IsInit) return;

    for(auto& vbo : m_Vbos)
    {
        if(vbo != 0)
        glDeleteBuffers(1, &vbo);
    }
	glDeleteVertexArrays(1, &m_Vao);
	m_IsInit = false;
}
void CMesh::CalculateBoudnigBox(const std::vector<float>& positions)
{
	Utils::CalculateBoudnigBox(positions, m_BoundingBox.m_BoundingBoxMin, m_BoundingBox.m_BoundingBoxMax, m_BoundingBox.m_BoundingSize, m_BoundingBox.m_BoundingCenter);
}

void CMesh::UpdateVertexPosition(const std::vector<float>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbos[VertexBufferObjects::POSITION]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
}

void CMesh::CreateVaoMesh()
{
	m_Vao = Utils::CreateVao();

    if (m_Indices.size() > 0)
    {
        GLuint vboId = Utils::BindIndicesBuffer(m_Indices);
        m_Vbos[VertexBufferObjects::INDICES] = vboId;
    }

	if (m_Positions.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(0, 3, m_Positions);
		m_Vbos[VertexBufferObjects::POSITION] = vboId;
	}
	if (m_TextCoords.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(1, 2, m_TextCoords);
		m_Vbos[VertexBufferObjects::TEXT_COORD] = vboId;
	}
	if (m_Normals.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(2, 3, m_Normals);
		m_Vbos[VertexBufferObjects::NORMAL] = vboId;
	}
	if (m_Tangents.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(3, 3, m_Tangents);
		m_Vbos[VertexBufferObjects::TANGENT] = vboId;
	}
	Utils::UnbindVao();
	m_IsInit = true;
}

void CMesh::CreateTransformsVbo(std::vector<glm::mat4>& m)
{
	glBindVertexArray(m_Vao);
	glGenBuffers(1, &m_Vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbos[VertexBufferObjects::TRANSFORM_MATRIX]);

	for (uint i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(4 + i, 1);
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m.size(), &m[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_TransformVboCreated = true;
}

void CMesh::UpdateTransformVbo(std::vector<glm::mat4>& m)
{
	glDeleteBuffers(1, &m_Vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
	glBindVertexArray(m_Vao);
	glGenBuffers(1, &m_Vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
	for (uint i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(4 + i, 1);
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m.size(), &m[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CMesh::CreateBoneVbo(const std::vector<SVertexBoneData>& bones)
{
	glBindVertexArray(m_Vao);
	glGenBuffers(1, &m_Vbos[VertexBufferObjects::BONES]);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbos[VertexBufferObjects::BONES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(8);
	glVertexAttribIPointer(8, 4, GL_INT, sizeof(SVertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(SVertexBoneData), (const GLvoid*)16);
	glBindVertexArray(0);
	m_BonesInShader = true;
}

void CMesh::SetInstancedMatrixes(const std::vector<glm::mat4>& m)
{
	m_InstancedMatrixes = m;
}

void CMesh::OpenGLLoadingPass()
{
	CreateVaoMesh();

	if (!m_Bones.empty())
	{
		CreateBoneVbo(m_Bones);
	}

	ClearData();
}

void CMesh::OpenGLPostLoadingPass()
{
	CreateTransformsVbo(m_InstancedMatrixes);
}

const GLuint& CMesh::GetVao() const
{
	return m_Vao;
}

const GLuint & CMesh::GetVbo(VertexBufferObjects::Type type) const
{
	return m_Vbos[type];
}

const SMaterial & CMesh::GetMaterial() const
{
	return m_Material;
}

void CMesh::ClearData()
{
	m_Positions.clear();
	m_TextCoords.clear();
	m_Normals.clear();
	m_Tangents.clear();
	m_Indices.clear();
	m_Bones.clear();
	m_InstancedMatrixes.clear();
}

const glm::vec3 & CMesh::GetBoundingSize()
{
	return m_BoundingBox.m_BoundingSize;
}

const glm::vec3 & CMesh::GetBoundingMin()
{
	return m_BoundingBox.m_BoundingBoxMin;
}

const glm::vec3 & CMesh::GetBoundingMax()
{
	return m_BoundingBox.m_BoundingBoxMax;
}

const glm::vec3 & CMesh::GetBoundingCenter()
{
	return m_BoundingBox.m_BoundingCenter;
}
const GLuint& CMesh::GetVertexCount() const
{
	return m_VertexCount;
}
