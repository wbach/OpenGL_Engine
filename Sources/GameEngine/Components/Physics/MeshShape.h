#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
class Model;

namespace Components
{
class MeshShape : public CollisionShape
{
public:
    MeshShape(const ComponentContext& componentContext, GameObject& gameObject);
    void Update();
    virtual void ReqisterFunctions() override;
    void OnAwake();

public:
    void SetModel(Model* model);

private:
    vec3 size_;
    Model* model_;

public:
    static ComponentsType type;
};
}  // Components
}  // GameEngine
