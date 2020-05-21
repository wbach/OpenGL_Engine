#pragma once
#include "CollisionShape.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class Model;

namespace Components
{
class MeshShape : public CollisionShape
{
public:
    MeshShape(ComponentContext& componentContext, GameObject& gameObject);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Update();
    void OnAwake();

public:
    MeshShape& SetSize(float size);
    MeshShape& SetModel(Model* model);
    MeshShape& SetModel(const File&);
    const Model* GetModel() const;

    inline float GetSize() const;

private:
    float size_;
    Model* model_;

public:
    static ComponentsType type;
};
float MeshShape::GetSize() const
{
    return size_;
}
}  // Components
}  // GameEngine
