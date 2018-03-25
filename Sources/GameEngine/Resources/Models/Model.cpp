#include "Model.h"
#include "Logger/Log.h"
#include "GLM/GLMUtils.h"

CModel::CModel()
	: CModel(1.f)
{
}

CModel::CModel(float scaleFactor)
	: scaleFactor_(scaleFactor)
{
}

void CModel::InitModel(const std::string& file_name)
{
	filename = file_name;
}

CModel::~CModel()
{
    Log(filename + " ::~CModel()");
}

void CModel::OpenGLLoadingPass()
{
	for (auto& mesh : meshes)
		mesh.OpenGLLoadingPass();

	COpenGLObject::OpenGLLoadingPass();
}

CMesh* CModel::AddMesh(CMesh& mesh)
{
	meshes.push_back(std::move(mesh));
	return &meshes.back();
}

const std::vector<mat4*>& CModel::GetBoneTransforms()
{
	if (!boneTransforms.empty() || skeleton_.size == 0)
		return boneTransforms;

	boneTransforms.resize(skeleton_.size);
	AddJoints(skeleton_);
	return boneTransforms;
}

void CModel::AddJoints(GameEngine::Animation::Joint& joint)
{
	boneTransforms[joint.id] = &joint.animatedTransform;
	for (auto& childJoint : joint.children)
	{
		AddJoints(childJoint);
	}
}
