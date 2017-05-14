#pragma once
#include "../GameObject.h"
#include <string>

class CModel;
class CResourceManager;

class CEntity : public CGameObject
{
public:
	CEntity();
	CEntity(CResourceManager& manager, const glm::vec3& normalized_scale, const std::string& filename, const std::string& filename2 = "", const std::string& filename3 = "");
	CEntity(const glm::vec3 normalized_v, CModel* model_lvl_1, CModel* model_lvl_2 = nullptr, CModel* model_lvl_3 = nullptr);

	bool dynamic = false;

	virtual ~CEntity() {}	
	CModel* GetModel(uint i);
private:
	void LoadModel(CResourceManager& manager, const glm::vec3& normalized_scale, const std::string& filename, uint i);

	CModel* m_Model[3];
};