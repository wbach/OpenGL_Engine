#pragma once
#include "EntityShader.h"
#include "GameEngine/Resources/Models/Material.h"

namespace GameEngine
{
class DefferedEntityShader : public EntityShader
{
public:
    DefferedEntityShader(IGraphicsApi& graphicsApi);

private:
    void GetAllUniformLocations() override;
    void ConnectTextureUnits() const override;
    void BindAttributes();
};
}  // namespace GameEngine
