#pragma once
#include <unordered_map>
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

static const uint32 MAX_LIGHTS = 10;

namespace GameEngine
{
class Light;

class DefferedShader : public ShaderProgram
{
private:
    struct LightParamsLocations
    {
        int type;
        int position;
        int colour;
        int attenuation;
        int direction;
        int cutOff;
    };

public:
    DefferedShader(IGraphicsApi& graphicsApi);

    void LoadLight(uint32 index, const Light& light);

private:
    virtual void BindAttributes() override;
    virtual void GetAllUniformLocations() override;
    virtual void ConnectTextureUnits() const override;

protected:
    LightParamsLocations lightParamsLocations_[MAX_LIGHTS];
};
}  // GameEngine
