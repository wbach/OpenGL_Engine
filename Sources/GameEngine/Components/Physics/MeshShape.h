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
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static const std::string name;
};
float MeshShape::GetSize() const
{
    return size_;
}
}  // namespace Components
}  // namespace GameEngine
