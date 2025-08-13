#pragma once
#include <atomic>
#include <memory>

#include "ComponentContext.h"
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include "IComponent.h"

// clang-format off
#define BEGIN_FIELDS() \
    std::vector<FieldInfo> GetFields() override { \
        std::vector<FieldInfo> fields;

#define FIELD_UINT(member) fields.push_back({#member, FieldType::UInt, &member});
#define FIELD_INT(member) fields.push_back({#member, FieldType::Int, &member});
#define FIELD_FLOAT(member) fields.push_back({#member, FieldType::Float, &member});
#define FIELD_STRING(member) fields.push_back({#member, FieldType::String, &member});
#define FIELD_BOOL(member) fields.push_back({#member, FieldType::Bool, &member});
#define FIELD_TEXTURE(member) fields.push_back({#member, FieldType::Texture, &member});
#define FIELD_FILE(member) fields.push_back({#member, FieldType::File, &member});
#define FIELD_VECTOR_OF_STRINGS(member) fields.push_back({#member, FieldType::VectorOfStrings, &member});
#define FIELD_VECTOR_OF_INTS(member) fields.push_back({#member, FieldType::VectorOfInt, &member});
#define FIELD_VECTOR_OF_FLOATS(member) fields.push_back({#member, FieldType::VectorOfFloat, &member});
#define FIELD_VECTOR_OF_FILES(member) fields.push_back({#member, FieldType::VectorOfFiles, &member});
#define FIELD_VECTOR_OF_TEXTURES(member) fields.push_back({#member, FieldType::VectorOfTextures, &member});

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
