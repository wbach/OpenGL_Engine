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
    MeshShape& SetSize(float size);
    MeshShape& SetModel(Model* model);
    MeshShape& SetModel(const std::string& filename);
    inline float GetSize() const;
    const std::string& GetModelFileName() const;

private:
    float size_;
    Model* model_;
    std::string modelFileName_;

public:
    static ComponentsType type;
};
float MeshShape::GetSize() const
{
    return size_;
}
}  // Components
}  // GameEngine
