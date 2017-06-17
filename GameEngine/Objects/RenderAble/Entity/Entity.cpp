#include "Entity.h"
#include "Logger/Log.h"
#include "../../../Resources/Models/Model.h"
#include "../../../Resources/ResourceManager.h"

CEntity::CEntity()
{
    for (auto& m : model)
        m = nullptr;
}

CEntity::CEntity(CResourceManager & manager, const glm::vec3 & normalized_scale, const std::string& filename, const std::string& filename2, const std::string& filename3)
{
	LoadModel(manager, normalized_scale, filename, 0);
	LoadModel(manager, normalized_scale, filename2, 1);
	LoadModel(manager, normalized_scale, filename3, 2);
}

CEntity::CEntity(const glm::vec3 normalized_v, CModel * model_lvl_1, CModel * model_lvl_2, CModel * model_lvl_3)
{
    model[0] = model_lvl_1;
    model[1] = model_lvl_2;
    model[2] = model_lvl_3;
    worldTransform.SetNormalizedSize(normalized_v);
}

CModel * CEntity::GetModel(uint32 i)
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

    if (model[i] == nullptr)
        return model[0];

    return model[i];
}

void CEntity::LoadModel(CResourceManager & manager, const glm::vec3 & normalized_scale, const std::string & filename, uint32 i)
{
	if (!filename.empty())
	{
        model[i] = manager.LoadModel(filename);
        manager.GetOpenGlLoader().AddObjectToOpenGLLoadingPass(model[i]);
        model[i]->CalculateBoudnigBox();
        auto vec = model[i]->GetNormalizedScaleVector(normalized_scale.x, normalized_scale.y, normalized_scale.z);
        worldTransform.SetNormalizedSize(vec);
	}
}
