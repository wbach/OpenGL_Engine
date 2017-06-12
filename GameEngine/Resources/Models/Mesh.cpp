#include "Mesh.h"
#include "../../Utils/GLM/GLMUtils.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"

CMesh::CMesh()
{
}

CMesh::CMesh(
    const SMaterial & mat,
    const std::vector<float>& pos,
	const std::vector<float>& text_coords,
    const std::vector<float>& norm,
    const std::vector<float>& tang,
    const std::vector<uint16>& ind,
    const std::vector<SVertexBoneData>& bon,
	const glm::mat4& mtransform
)
{
    positions    = std::move(pos);
    textCoords   = std::move(text_coords);
    normals      = std::move(norm);
    tangents	 = std::move(tang);
    indices	     = std::move(ind);
    bones		 = std::move(bon);
    material	 = mat;
	transform	 = mtransform;
    vertexCount  = indices.size() > 0 ? indices.size() : positions.size() / 3;

    if (!positions.empty())	attributes.push_back(0);
    if (!textCoords.empty())	attributes.push_back(1);
    if (!normals.empty())		attributes.push_back(2);
    if (!tangents.empty())	attributes.push_back(3);

    CalculateBoudnigBox(positions);
}

CMesh::~CMesh()
{
    if (!isInit) return;

    for(auto& vbo : vbos)
    {
        if(vbo != 0)
        glDeleteBuffers(1, &vbo);
    }
    glDeleteVertexArrays(1, &vao);
    isInit = false;
}
void CMesh::CalculateBoudnigBox(const std::vector<float>& positions)
{
    Utils::CalculateBoudnigBox(positions, boundingBox.min, boundingBox.max, boundingBox.size, boundingBox.center);
}

void CMesh::UpdateVertexPosition(const std::vector<float>& vertices) const
{
    glBindBuffer(GL_ARRAY_BUFFER, vbos[VertexBufferObjects::POSITION]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
}

void CMesh::CreateVaoMesh()
{
    vao = Utils::CreateVao();

    if (indices.size() > 0)
    {
        GLuint vboId = Utils::BindIndicesBuffer(indices);
        vbos[VertexBufferObjects::INDICES] = vboId;
    }

    if (positions.size() > 0)
	{
        GLuint vboId = Utils::StoreDataInAttributesList(0, 3, positions);
        vbos[VertexBufferObjects::POSITION] = vboId;
	}
    if (textCoords.size() > 0)
	{
        GLuint vboId = Utils::StoreDataInAttributesList(1, 2, textCoords);
        vbos[VertexBufferObjects::TEXT_COORD] = vboId;
	}
    if (normals.size() > 0)
	{
        GLuint vboId = Utils::StoreDataInAttributesList(2, 3, normals);
        vbos[VertexBufferObjects::NORMAL] = vboId;
	}
    if (tangents.size() > 0)
	{
        GLuint vboId = Utils::StoreDataInAttributesList(3, 3, tangents);
        vbos[VertexBufferObjects::TANGENT] = vboId;
	}
	Utils::UnbindVao();
    isInit = true;
}

void CMesh::CreateTransformsVbo(std::vector<glm::mat4>& m)
{
    glBindVertexArray(vao);
    glGenBuffers(1, &vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[VertexBufferObjects::TRANSFORM_MATRIX]);

	for (uint32 i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(4 + i, 1);
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m.size(), &m[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    transformVboCreated = true;
}

void CMesh::UpdateTransformVbo(std::vector<glm::mat4>& m)
{
    glDeleteBuffers(1, &vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
	for (uint32 i = 0; i < 4; i++)
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
    glBindVertexArray(vao);
    glGenBuffers(1, &vbos[VertexBufferObjects::BONES]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[VertexBufferObjects::BONES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(8);
	glVertexAttribIPointer(8, 4, GL_INT, sizeof(SVertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(SVertexBoneData), (const GLvoid*)16);
	glBindVertexArray(0);
    bonesInShader = true;
}

void CMesh::SetInstancedMatrixes(const std::vector<glm::mat4>& m)
{
    instancedMatrixes = m;
}

void CMesh::OpenGLLoadingPass()
{
    if (isInOpenGL())
        return;

	CreateVaoMesh();

    if (!bones.empty())
	{
        CreateBoneVbo(bones);
	}

	ClearData();

    COpenGLObject::OpenGLLoadingPass();
}

void CMesh::OpenGLPostLoadingPass()
{
    CreateTransformsVbo(instancedMatrixes);
}

const GLuint& CMesh::GetVao() const
{
    return vao;
}

const GLuint & CMesh::GetVbo(VertexBufferObjects::Type type) const
{
    return vbos[type];
}

const SMaterial & CMesh::GetMaterial() const
{
    return material;
}

void CMesh::SetMaterial(const SMaterial& mat)
{
    material = mat;
}

void CMesh::ClearData()
{
    positions.clear();
    textCoords.clear();
    normals.clear();
    tangents.clear();
    indices.clear();
    bones.clear();
    instancedMatrixes.clear();
}

const glm::vec3 & CMesh::GetBoundingSize()
{
    return boundingBox.size;
}

const glm::vec3 & CMesh::GetBoundingMin()
{
    return boundingBox.min;
}

const glm::vec3 & CMesh::GetBoundingMax()
{
    return boundingBox.max;
}

const glm::vec3 & CMesh::GetBoundingCenter()
{
    return boundingBox.center;
}
const GLuint& CMesh::GetVertexCount() const
{
    return vertexCount;
}
