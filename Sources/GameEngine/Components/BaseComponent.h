#pragma once
#include <atomic>
#include <memory>

#include "ComponentContext.h"
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include "IComponent.h"
#include "ReadAnimationInfo.h"

// clang-format off
#define BEGIN_FIELDS() \
    std::vector<FieldInfo> GetFields() override { \
        std::vector<FieldInfo> fields;

#define FIELD_UINT(member) fields.push_back({#member, FieldType::UInt, &(this->member)});
#define FIELD_INT(member) fields.push_back({#member, FieldType::Int, &(this->member)});
#define FIELD_FLOAT(member) fields.push_back({#member, FieldType::Float, &(this->member)});
#define FIELD_STRING(member) fields.push_back({#member, FieldType::String, &(this->member)});
#define FIELD_BOOL(member) fields.push_back({#member, FieldType::Boolean, &(this->member)});
#define FIELD_TEXTURE(member) fields.push_back({#member, FieldType::Texture, &(this->member)});
#define FIELD_FILE(member) fields.push_back({#member, FieldType::File, &(this->member)});
#define FIELD_ANIMATION_CLIP(member) fields.push_back({#member, FieldType::AnimationClip, &(this->member)});
#define FIELD_VECTOR2I(member) fields.push_back({#member, FieldType::Vector2i, &(this->member)});
#define FIELD_VECTOR2F(member) fields.push_back({#member, FieldType::Vector2f, &(this->member)});
#define FIELD_VECTOR3F(member) fields.push_back({#member, FieldType::Vector3f, &(this->member)});
#define FIELD_VECTOR4F(member) fields.push_back({#member, FieldType::Vector4f, &(this->member)});
#define FIELD_COLOR_RGB(member) fields.push_back({#member, FieldType::ColorRGB, &(this->member)});
#define FIELD_COLOR_RGBA(member) fields.push_back({#member, FieldType::ColorRGBA, &(this->member)});
#define FIELD_VECTOR_OF_STRINGS(member) fields.push_back({#member, FieldType::VectorOfStrings, &(this->member)});
#define FIELD_VECTOR_OF_INTS(member) fields.push_back({#member, FieldType::VectorOfInt, &(this->member)});
#define FIELD_VECTOR_OF_FLOATS(member) fields.push_back({#member, FieldType::VectorOfFloat, &(this->member)});
#define FIELD_VECTOR_OF_FILES(member) fields.push_back({#member, FieldType::VectorOfFiles, &(this->member)});
#define FIELD_VECTOR_OF_TEXTURES(member) fields.push_back({#member, FieldType::VectorOfTextures, &(this->member)});
#define FIELD_VECTOR_OF_ANIMATION_CLIPS(member) fields.push_back({#member, FieldType::VectorOfAnimationClips, &(this->member)});

#define END_FIELDS() return fields; }
// clang-format on

namespace GameEngine
{
class GameObject;

namespace Components
{
class BaseComponent : public IComponent
{
public:
    BaseComponent(size_t, ComponentContext&, GameObject&);
    ~BaseComponent() override;

    bool IsActive() const override;
    void Activate() override;
    void Deactivate() override;
    void SetActive(bool) override;

    void InitFromParams(const std::unordered_map<std::string, std::string>&) override;
    std::unordered_map<ParamName, Param> GetParams() const override;

    GameObject& GetParentGameObject() override;
    const GameObject& getParentGameObject() const override;

    void write(TreeNode&) const override;
    std::optional<IdType> getRegisteredFunctionId(FunctionType) const override;
    std::vector<FieldInfo> GetFields() override
    {
        return {};
    }

protected:
    void RegisterFunction(FunctionType, std::function<void()>);

protected:
    size_t type_;
    GameObject& thisObject_;
    ComponentContext& componentContext_;

private:
    void changeActivateStateRegisteredFunctions();

private:
    bool isActive_;
    std::unordered_map<uint32, FunctionType> registeredFunctionsIds_;
    std::optional<uint32> componentRegistredId_;
};
}  // namespace Components
}  // namespace GameEngine
