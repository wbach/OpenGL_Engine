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
    MeshShape& SetModel(Model* model);
    MeshShape& SetSize(float size);

private:
    float size_;
    Model* model_;

public:
    static ComponentsType type;
};
}  // Components
}  // GameEngine
