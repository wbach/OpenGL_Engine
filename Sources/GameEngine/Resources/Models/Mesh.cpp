#include "Mesh.h"
#include "GLM/GLMUtils.h"

CMesh::CMesh()
{
}

CMesh::CMesh(GameEngine::IGraphicsApiPtr graphicsApi, const SMaterial& material, const mat4& transformMatix)
	: graphicsApi_(graphicsApi)
	, material_(material)
	, transform_(transformMatix)
{
}

CMesh::~CMesh()
{
	if (!isInit) return;

	graphicsApi_->DeleteObject(objectId_);
}
void CMesh::CalculateBoudnigBox(const std::vector<float>& positions)
{
	Utils::CalculateBoudnigBox(positions, boundingBox.min, boundingBox.max, boundingBox.size, boundingBox.center);
}

void CMesh::CreateMesh()
{
	useAramture = !meshRawData_.bonesWeights_.empty() && !meshRawData_.joinIds_.empty();	
	objectId_ = graphicsApi_->CreateMesh(meshRawData_);
	isInit = true;
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

	CreateMesh();
	ClearData();

	COpenGLObject::OpenGLLoadingPass();
}

void CMesh::OpenGLPostLoadingPass()
{
}

void CMesh::SetTransformMatrix(const glm::mat4 & m)
{
	transform_ = m;
}

uint32 CMesh::GetObjectId() const
{
	return objectId_;
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

