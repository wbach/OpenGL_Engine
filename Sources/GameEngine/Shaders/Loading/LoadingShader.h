#pragma once
#include <vector>
#include "../ShaderProgram.h"

namespace GameEngine
{
class LoadingShader : public ShaderProgram
{
public:
    LoadingShader(IGraphicsApiPtr);
    void ConnectTextureUnits() const override;

    void GetAllUniformLocations() override;
    void BindAttributes() override;
};
}  // namespace GameEngine
