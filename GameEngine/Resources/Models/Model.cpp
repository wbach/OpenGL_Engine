#include "Model.h"
#include "../../Debug_/Log.h"
#include "../../Utils/GLM/GLMUtils.h"

CModel::CModel()
{

}

void CModel::InitModel(const std::string& file_name)
{
	filename = file_name;
	Log(file_name + " succesful loaded");
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

CMesh * CModel::AddMesh(const SMaterial& material, const std::vector<float>& positions, const std::vector<float>& text_coords, const std::vector<float>& normals, const std::vector<float>& tangents, const std::vector<uint16>& indices,  const std::vector<SVertexBoneData>& bones)
{
    //CMesh mesh(positions, text_coords, normals, tangents, indices, material, bones);
	// Normalize to define scale (height) 1 unit = 1 metr
    meshes.emplace_back(material, positions, text_coords, normals, tangents, indices, bones);
	CalculateBoudnigBox();
	return &meshes.back();
}

CMesh * CModel::AddMesh(CMesh& mesh)
{
	meshes.push_back(std::move(mesh));
	CalculateBoudnigBox();
	return &meshes.back();
}

void CModel::SetMaterial(const SMaterial & material, uint32 id)
{
	if (id == 0)
	{
		for (auto& mesh : meshes)
		{
			mesh.SetMaterial(material);
		}
	}
	else
	{
        uint32 x = 0;
		for (auto& mesh : meshes)
		{
			if (x == id)
			{
				mesh.SetMaterial(material);
				return;
			}
		}
	}
}

glm::vec3 CModel::GetNormalizedScaleVector(float w, float h, float z) const
{
	int axis;
	if (w == 0 && h == 0 && z == 0)
		return glm::vec3(1);
	else if (w != 0 && h == 0 && z == 0)
		axis = 1;
	else if (w == 0 && h != 0 && z == 0)
		axis = 2;
	else if (w == 0 && h == 0 && z != 0)
		axis = 3;


	glm::vec3 scale_vector(1.f);
	switch (axis)
	{
	case 1:
		scale_vector = glm::vec3(w / boundingBox.size.x, w / boundingBox.size.x, w / boundingBox.size.x);
		break;
	case 2: scale_vector = glm::vec3(h / boundingBox.size.y, h / boundingBox.size.y, h / boundingBox.size.y);
		break;
	case 3: scale_vector = glm::vec3(z / boundingBox.size.z, z / boundingBox.size.z, z / boundingBox.size.z);
		break;
	default: scale_vector = glm::vec3(w / boundingBox.size.x, h / boundingBox.size.y, z / boundingBox.size.z);
		break;
	}
		
	return scale_vector;
	
}

void CModel::CalculateBoudnigBox()
{
	bool first = true;
	for (auto& mesh : meshes)
	{
		if (first)
		{
			boundingBox.min = mesh.GetBoundingMin();
			boundingBox.max = mesh.GetBoundingMax();
			first = false;
		}
		else
		{
			boundingBox.min = Utils::CalculateMinimumVector(boundingBox.min, mesh.GetBoundingMin());
			boundingBox.max = Utils::CalculateMinimumVector(mesh.GetBoundingMax(), boundingBox.max);
			boundingBox.max = Utils::CalculateMinimumVector(boundingBox.max, mesh.GetBoundingMax());
		}
	}
	boundingBox.size = boundingBox.max - boundingBox.min;
	boundingBox.center = (boundingBox.max + boundingBox.min) / 2.f;
}

