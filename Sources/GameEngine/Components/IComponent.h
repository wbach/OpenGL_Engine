#pragma once
#include <Types.h>

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "FunctionType.h"

class TreeNode;

namespace GameEngine
{
class GameObject;

namespace Components
{
enum class FieldType
{
    Int = 0,
    UInt,
    Float,
    String,
    Boolean,
    Texture,
    File,
    AnimationClip,
    Vector2i,
    Vector2f,
    Vector3f,
    Vector4f,
    ColorRGB,
    ColorRGBA,
    Enum,
    VectorOfStrings,
    VectorOfInt,
    VectorOfFloat,
    VectorOfFiles,
    VectorOfTextures,
    VectorOfAnimationClips,
    ConstVectorOfTextures
};

struct FieldInfo
{
    std::string name;
    FieldType type;
    void* ptr;

    std::function<std::vector<std::string>()> enumNames;
    std::function<int(void*)> enumToIndex;
    std::function<void(void*, int)> indexToEnum;
};

typedef std::string ParamName;

class IComponent
{
public:
    using Type = uint64_t;

    virtual ~IComponent()                                                     = default;
    virtual void CleanUp()                                                    = 0;
    virtual void ReqisterFunctions()                                          = 0;
    virtual bool IsActive() const                                             = 0;
    virtual void Activate()                                                   = 0;
    virtual void SetActive(bool)                                              = 0;
    virtual void Deactivate()                                                 = 0;
    virtual GameObject& GetParentGameObject()                                 = 0;
    virtual const GameObject& getParentGameObject() const                     = 0;
    virtual void write(TreeNode&) const                                       = 0;
    virtual std::optional<IdType> getRegisteredFunctionId(FunctionType) const = 0;
    virtual std::vector<FieldInfo> GetFields()                                = 0;
    virtual Type GetType() const                                              = 0;
    virtual const std::string& GetTypeString() const                          = 0;
};
}  // namespace Components
}  // namespace GameEngine
