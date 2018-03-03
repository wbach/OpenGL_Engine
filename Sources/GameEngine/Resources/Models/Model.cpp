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

CMesh* CModel::AddMesh(const SMaterial& material, const std::vector<float>& positions, const std::vector<float>& text_coords, const std::vector<float>& normals, const std::vector<float>& tangents, const std::vector<uint16>& indices,  const std::vector<SVertexBoneData>& bones)
{
    meshes.emplace_back(material, positions, text_coords, normals, tangents, indices, bones);
	return &meshes.back();
}

CMesh* CModel::AddMesh(CMesh& mesh)
{
	meshes.push_back(std::move(mesh));
	return &meshes.back();
}
