#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
namespace Components
{
class WaterRendererComponent : public BaseComponent
{
public:
    WaterRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);

    const vec3& GetPosition() const;
    const vec3& GetScale() const;

private:
    void ReqisterFunctions();
    void Subscribe();
    void UnSubscribe();

private:
    vec3 position_;
    vec3 scale_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
