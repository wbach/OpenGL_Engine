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
    File modelFile;
    float size;
    bool autoOptimization;
    bool modelNormalization;
    bool meshOptimize;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(modelFile)
        FIELD_BOOL(modelNormalization)
        FIELD_BOOL(meshOptimize)
        FIELD_BOOL(autoOptimization)
        FIELD_FLOAT(size)
        FIELD_VECTOR3F(positionOffset)
    END_FIELDS()
    // clang-format on

public:
    MeshShape(ComponentContext& componentContext, GameObject& gameObject);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void InitShape() override;
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
    Model* model_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static const std::string name;
};
float MeshShape::GetSize() const
{
    return size;
}
}  // namespace Components
}  // namespace GameEngine
