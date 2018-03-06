#include "Model.h"

#include "Logger/Log.h"
#include "GLM/GLMUtils.h"

CModel::CModel()
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
