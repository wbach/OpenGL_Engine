#include "Mesh.h"
#include "GLM/GLMUtils.h"
#include "OpenGL/OpenGLUtils.h"

CMesh::CMesh()
{
}

CMesh::CMesh(const SMaterial& material, const mat4& transformMatix)
	: material_(material)
	, transform_(transformMatix)
{
}

CMesh::~CMesh()
{
	if (!isInit) return;

	for (auto& vbo : vbos_)
	{
		if (vbo.second != 0)
			glDeleteBuffers(1, &vbo.second);
	}
	vbos_.clear();
	glDeleteVertexArrays(1, &vao_);
	isInit = false;
}
void CMesh::CalculateBoudnigBox(const std::vector<float>& positions)
{
	Utils::CalculateBoudnigBox(positions, boundingBox.min, boundingBox.max, boundingBox.size, boundingBox.center);
}

void CMesh::UpdateVertexPosition(const std::vector<float>& vertices) const
{
	glBindBuffer(GL_ARRAY_BUFFER, vbos_.at(VertexBufferObjects::POSITION));
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
}

void CMesh::CreateVaoMesh()
{
	useAramture = !meshRawData_.bonesWeights_.empty() && !meshRawData_.joinIds_.empty();
	vertexCount_ = meshRawData_.indices_.size() > 0 ? meshRawData_.indices_.size() : meshRawData_.positions_.size() / 3;

	vao_ = Utils::CreateVao();

	if (meshRawData_.indices_.size() > 0)
	{
		GLuint vboId = Utils::BindIndicesBuffer(meshRawData_.indices_);
		vbos_[VertexBufferObjects::INDICES] = vboId;
	}

	if (meshRawData_.positions_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(0, 3, meshRawData_.positions_);
		vbos_[VertexBufferObjects::POSITION] = vboId;
		attributes_[VertexBufferObjects::POSITION] = 0;
	}
	if (meshRawData_.textCoords_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(1, 2, meshRawData_.textCoords_);
		vbos_[VertexBufferObjects::TEXT_COORD] = vboId;
		attributes_[VertexBufferObjects::TEXT_COORD] = 1;
	}
	if (meshRawData_.normals_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(2, 3, meshRawData_.normals_);
		vbos_[VertexBufferObjects::NORMAL] = vboId;
		attributes_[VertexBufferObjects::NORMAL] = 2;
	}
	if (meshRawData_.tangents_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(3, 3, meshRawData_.tangents_);
		vbos_[VertexBufferObjects::TANGENT] = vboId;
		attributes_[VertexBufferObjects::TANGENT] = 3;
	}
	if (meshRawData_.bonesWeights_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(4, 3, meshRawData_.bonesWeights_);
		vbos_[VertexBufferObjects::WEIGHTS] = vboId;
		attributes_[VertexBufferObjects::WEIGHTS] = 4;
	}
	if (meshRawData_.joinIds_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(5, 3, meshRawData_.joinIds_);
		vbos_[VertexBufferObjects::JOINTS] = vboId;
		attributes_[VertexBufferObjects::JOINTS] = 5;
	}

	Utils::UnbindVao();
	isInit = true;
}

void CMesh::CreateTransformsVbo(std::vector<mat4>& m)
{
	/* glBindVertexArray(vao_);
	 glGenBuffers(1, &vbos_[VertexBufferObjects::TRANSFORM_MATRIX]);
	 glBindBuffer(GL_ARRAY_BUFFER, vbos_[VertexBufferObjects::TRANSFORM_MATRIX]);

	 for (uint32 i = 0; i < 4; i++)
	 {
		 glEnableVertexAttribArray(4 + i);
		 glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		 glVertexAttribDivisor(4 + i, 1);
	 }
	 glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m.size(), &m[0], GL_STATIC_DRAW);
	 glBindBuffer(GL_ARRAY_BUFFER, 0);
	 glBindVertexArray(0);
	 transformVboCreated = true;*/
}

void CMesh::UpdateTransformVbo(std::vector<mat4>& m)
{
	//   glDeleteBuffers(1, &vbos_[VertexBufferObjects::TRANSFORM_MATRIX]);
	//   glBindVertexArray(vao_);
	//   glGenBuffers(1, &vbos_[VertexBufferObjects::TRANSFORM_MATRIX]);
	//   glBindBuffer(GL_ARRAY_BUFFER, vbos_[VertexBufferObjects::TRANSFORM_MATRIX]);
	   //for (uint32 i = 0; i < 4; i++)
	   //{
	   //	glEnableVertexAttribArray(4 + i);
	   //	glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
	   //	glVertexAttribDivisor(4 + i, 1);
	   //}
	   //glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m.size(), &m[0], GL_STATIC_DRAW);
	   //glBindBuffer(GL_ARRAY_BUFFER, 0);
	   //glBindVertexArray(0);
}

void CMesh::SetInstancedMatrixes(const std::vector<mat4>& m)
{
	meshRawData_.instancedMatrixes_ = m;
}

bool CMesh::IsInit() const
{
	return isInit;
}

bool CMesh::UseArmature() const
{
	return useAramture;
}

void CMesh::OpenGLLoadingPass()
{
	if (isInOpenGL())
		return;

	CreateVaoMesh();
	ClearData();

	COpenGLObject::OpenGLLoadingPass();
}

void CMesh::OpenGLPostLoadingPass()
{
	CreateTransformsVbo(meshRawData_.instancedMatrixes_);
}

const GLuint& CMesh::GetVao() const
{
	return vao_;
}

GLuint CMesh::GetVbo(VertexBufferObjects type) const
{
	return vbos_.at(type);
}

const SMaterial & CMesh::GetMaterial() const
{
	return material_;
}

void CMesh::SetMaterial(const SMaterial& mat)
{
	material_ = mat;
}

void CMesh::ClearData()
{
	meshRawData_ = GameEngine::MeshRawData();
}

const BoundingBox& CMesh::GetBoundingBox() const
{
	return boundingBox;
}

const GLuint& CMesh::GetVertexCount() const
{
	return vertexCount_;
}
