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
    void setScale(const vec3&) override;

public:
    MeshShape& SetSize(float size);
    MeshShape& SetModel(Model* model);
    MeshShape& SetModel(const File&);

    void autoOptimize();
    const Model* GetModel() const;
    inline float GetSize() const;

private:
    vec3 calculateScale(const vec3&) const;

private:
    float size_;
    Model* model_;
    bool autoOptimize_;
    std::string requstedModelFileName_;

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
