#include "Mesh.h"
#include "GLM/GLMUtils.h"
#include "OpenGL/OpenGLUtils.h"

CMesh::CMesh()
{
}

CMesh::CMesh(
	const SMaterial& material,
	const FloatVec& positions,
	const FloatVec& text_coords,
	const FloatVec& normals,
	const FloatVec& tangents,
	const UintVec& indices,
	const Int32Vec& joinIds,
	const FloatVec& bonesWeights,
	const mat4& transform
)
{
	positions_ = std::move(positions);
	textCoords_ = std::move(text_coords);
	normals_ = std::move(normals);
	tangents_ = std::move(tangents);
	indices_ = std::move(indices);
	joinIds_ = std::move(joinIds);
	bonesWeights_ = std::move(bonesWeights);
	material_ = material;
	transform_ = transform;
	vertexCount_ = indices_.size() > 0 ? indices_.size() : positions_.size() / 3;

	if (!positions_.empty())	attributes_[VertexBufferObjects::POSITION] = 0;
	if (!textCoords_.empty())	attributes_[VertexBufferObjects::TEXT_COORD] = 1;
	if (!normals_.empty())		attributes_[VertexBufferObjects::NORMAL] = 2;
	if (!tangents_.empty())		attributes_[VertexBufferObjects::TANGENT] = 3;
	if (!bonesWeights_.empty())	attributes_[VertexBufferObjects::WEIGHTS] = 4;
	if (!joinIds_.empty())		attributes_[VertexBufferObjects::JOINTS] = 5;

	CalculateBoudnigBox(positions);
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
	vao_ = Utils::CreateVao();

	if (indices_.size() > 0)
	{
		GLuint vboId = Utils::BindIndicesBuffer(indices_);
		vbos_[VertexBufferObjects::INDICES] = vboId;
	}

	if (positions_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(0, 3, positions_);
		vbos_[VertexBufferObjects::POSITION] = vboId;
	}
	if (textCoords_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(1, 2, textCoords_);
		vbos_[VertexBufferObjects::TEXT_COORD] = vboId;
	}
	if (normals_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(2, 3, normals_);
		vbos_[VertexBufferObjects::NORMAL] = vboId;
	}
	if (tangents_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(3, 3, tangents_);
		vbos_[VertexBufferObjects::TANGENT] = vboId;
	}
	if (bonesWeights_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(4, 3, bonesWeights_);
		vbos_[VertexBufferObjects::WEIGHTS] = vboId;
	}
	if (joinIds_.size() > 0)
	{
		GLuint vboId = Utils::StoreDataInAttributesList(5, 3, joinIds_);
		vbos_[VertexBufferObjects::JOINTS] = vboId;
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
	instancedMatrixes_ = m;
}

bool CMesh::IsInit() const
{
	return isInit;
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
	CreateTransformsVbo(instancedMatrixes_);
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
	positions_.clear();
	textCoords_.clear();
	normals_.clear();
	tangents_.clear();
	indices_.clear();
	joinIds_.clear();
	bonesWeights_.clear();
	instancedMatrixes_.clear();
}

std::vector<mat4> CMesh::GetJointTransforms() const
{
	std::vector<mat4>out;
	out.resize(16);//!
	AddJoints(rootJoint_, out);
	return out;
}

void CMesh::AddJoints(GameEngine::Animation::Joint joint, std::vector<mat4>& m) const
{
	m[joint.id] = joint.animatedTransform;
	for (auto& childJoint : joint.children)
	{
		AddJoints(childJoint, m);
	}
}

const BoundingBox& CMesh::GetBoundingBox() const
{
	return boundingBox;
}

const GLuint& CMesh::GetVertexCount() const
{
	return vertexCount_;
}
