#pragma once
#include <atomic>
#include <magic_enum/magic_enum.hpp>
#include <memory>

#include "ComponentContext.h"
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include "IComponent.h"
#include "ReadAnimationInfo.h"

namespace GameEngine
{
class GameObject;

namespace Components
{
template <typename Enum>
FieldInfo MakeEnumField(const char* name, Enum* value)
{
    // cache nazw dla danego Enum
    static const std::vector<std::string> cachedNames = [] {
        std::vector<std::string> result;
        for (auto n : magic_enum::enum_names<Enum>())
            result.emplace_back(n);
        return result;
    }();

    return {
        .name = name,
        .type = FieldType::Enum,
        .ptr  = value,

        // zwracamy kopię – UI może to sobie przechować
        .enumNames = []() { return cachedNames; },

        .enumToIndex = [](void* ptr) {
            auto val = *static_cast<Enum*>(ptr);
            // nazwa wartości:
            auto sv  = magic_enum::enum_name(val);
            LOG_DEBUG << "Value: " << std::string(sv);

            // indeks w zakresie 0..N-1 (nie mylić z underlying!)
            if (auto idx = magic_enum::enum_index(val); idx.has_value())
                return static_cast<int>(*idx);
            // fallback (gdyby wartość była spoza zakresu)
            return 0;
        },

        .indexToEnum = [](void* ptr, int idx) {
            LOG_DEBUG << "Idx: " << idx;
            // clamp na wypadek złego indeksu
            constexpr int N = static_cast<int>(magic_enum::enum_count<Enum>());
            if (N > 0) {
                if (idx < 0) idx = 0;
                if (idx >= N) idx = N - 1;
            } else {
                idx = 0;
            }

            auto val = magic_enum::enum_value<Enum>(idx);
            *static_cast<Enum*>(ptr) = val;

            auto sv = magic_enum::enum_name(val);
            LOG_DEBUG << "Idx: " << idx << " -> Value: " << std::string(sv);
        }
    };
}

// Makro – przekazujemy #member (string literal), więc nie ma ryzyka wiszącego wskaźnika:
//#define FIELD_ENUM(member, EnumT) fields.push_back(MakeEnumField<EnumT>(#member, &(this->member)))

}  // namespace Components
}  // namespace GameEngine

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
//#define FIELD_ENUM(member) fields.push_back({#member, FieldType::Enum, &(this->member)});
#define FIELD_VECTOR_OF_STRINGS(member) fields.push_back({#member, FieldType::VectorOfStrings, &(this->member)});
#define FIELD_VECTOR_OF_INTS(member) fields.push_back({#member, FieldType::VectorOfInt, &(this->member)});
#define FIELD_VECTOR_OF_FLOATS(member) fields.push_back({#member, FieldType::VectorOfFloat, &(this->member)});
#define FIELD_VECTOR_OF_FILES(member) fields.push_back({#member, FieldType::VectorOfFiles, &(this->member)});
#define FIELD_VECTOR_OF_TEXTURES(member) fields.push_back({#member, FieldType::VectorOfTextures, &(this->member)});
#define FIELD_CONST_VECTOR_OF_TEXTURES(member) fields.push_back({#member, FieldType::ConstVectorOfTextures, &(this->member)});
#define FIELD_VECTOR_OF_ANIMATION_CLIPS(member) fields.push_back({#member, FieldType::VectorOfAnimationClips, &(this->member)});
#define FIELD_ENUM(member) fields.push_back(MakeEnumField(#member, &(this->member)));
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

    size_t GetType() const override;
    bool IsActive() const override;
    void Activate() override;
    void Deactivate() override;
    void SetActive(bool) override;

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
