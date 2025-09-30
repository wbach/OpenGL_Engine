#include "PerMeshObject.h"

#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
PerMeshObject createPerMeshObject(const Material& material)
{
    PerMeshObject perMeshObject;
    perMeshObject.ambient         = ToVec4(material.ambient);
    perMeshObject.diffuse         = ToVec4(material.diffuse);
    perMeshObject.specular        = ToVec4(material.specular);
    perMeshObject.shineDamper     = material.shineDamper;
    perMeshObject.useFakeLighting = material.useFakeLighting;

    if (material.diffuseTexture != nullptr)
    {
        perMeshObject.numberOfRows    = material.diffuseTexture->getNumberOfRows();
        perMeshObject.haveDiffTexture = 1.f;
    }
    else
    {
        perMeshObject.numberOfRows    = 1;
        perMeshObject.haveDiffTexture = 0.f;
    }

    if (material.normalTexture != nullptr)
    {
        perMeshObject.haveNormalMap = 1.f;
    }
    else
    {
        perMeshObject.haveNormalMap = 0.f;
    }

    if (material.specularTexture != nullptr)
    {
        perMeshObject.haveSpecularMap = 1.f;
    }
    else
    {
        perMeshObject.haveSpecularMap = 0.f;
    }
    return perMeshObject;
}
}  // namespace GameEngine