#include "Entity.h"
#include "../../Debug_/Log.h"
#include "../../Resources/Models/Model.h"
#include "../../Resources/ResourceManager.h"

CEntity::CEntity()
{
	for (auto& model : m_Model)
		model = nullptr;
}

CEntity::CEntity(CResourceManager & manager, const glm::vec3 & normalized_scale, const std::string& filename, const std::string& filename2, const std::string& filename3)
{
	LoadModel(manager, normalized_scale, filename, 0);
	LoadModel(manager, normalized_scale, filename2, 1);
	LoadModel(manager, normalized_scale, filename3, 2);
}

CEntity::CEntity(const glm::vec3 normalized_v, CModel * model_lvl_1, CModel * model_lvl_2, CModel * model_lvl_3)
{
	m_Model[0] = model_lvl_1;
	m_Model[1] = model_lvl_2;
	m_Model[2] = model_lvl_3;
	m_WorldTransform.SetNormalizedSize(normalized_v);
}

CModel * CEntity::GetModel(uint i)
{
	if (i > 2)
	{
		i = 2;
		Log("CEntity::GetModel out of range > 2");
	}
	if (i < 0)
	{
		i = 0;
		Log("CEntity::GetModel out of range < 0");
	}

	if (m_Model[i] == nullptr)
		return m_Model[0];

	return m_Model[i];
}

void CEntity::LoadModel(CResourceManager & manager, const glm::vec3 & normalized_scale, const std::string & filename, uint i)
{
	if (!filename.empty())
	{
		m_Model[i] = manager.LoadModel(filename);
		manager.GetOpenGlLoader().AddObjectToOpenGLLoadingPass(m_Model[i]);
		m_Model[i]->CalculateBoudnigBox();
		auto vec = m_Model[i]->GetNormalizedScaleVector(normalized_scale.x, normalized_scale.y, normalized_scale.z);
		m_WorldTransform.SetNormalizedSize(vec);
	}
}
