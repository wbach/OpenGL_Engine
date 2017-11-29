#pragma once
#include <string>
#include "../../GameObject.h"

class CModel;
class CResourceManager;

class CEntity : public CGameObject
{
public:
    CEntity();
    CEntity(CResourceManager& manager, const vec3& normalized_scale, const std::string& filename, const std::string& filename2 = "",
            const std::string& filename3 = "");
    CEntity(const vec3 normalized_v, CModel* model_lvl_1, CModel* model_lvl_2 = nullptr, CModel* model_lvl_3 = nullptr);

    virtual ~CEntity()
    {
    }
    CModel* GetModel(uint32 i);

public:
    bool dynamic = false;

private:
    void LoadModel(CResourceManager& manager, const vec3& normalized_scale, const std::string& filename, uint32 i);

    CModel* model[3];
};
