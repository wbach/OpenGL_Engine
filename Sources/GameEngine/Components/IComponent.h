#pragma once
#include <Types.h>

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
const std::string IMAGE_FILE{"imageFile"};
const std::string MODEL_FILE{"modelFile"};
const std::string FILE{"file"};
const std::string STRING{"string"};
const std::string INT{"int"};
const std::string BOOL{"bool"};
const std::string FLOAT{"float"};
const std::string VECTOR{"vector"};

struct Param
{
    std::string type;
    std::string value;
};

enum class FieldType
{
    Int,
    Float,
    String,
    Bool,
    Texture,
    File,
    VectorOfStrings,
    VectorOfInt,
    VectorOfFloat,
    VectorOfFiles,
    VectorOfTextures
};

struct FieldInfo
{
    const char* name;
    FieldType type;
    void* ptr;
};

typedef std::string ParamName;

class IComponent
{
public:
    using Type = size_t;

    virtual ~IComponent()                                                            = default;
    virtual void CleanUp()                                                           = 0;
    virtual void ReqisterFunctions()                                                 = 0;
    virtual bool IsActive() const                                                    = 0;
    virtual void Activate()                                                          = 0;
    virtual void Deactivate()                                                        = 0;
    virtual void InitFromParams(const std::unordered_map<std::string, std::string>&) = 0;
    virtual std::unordered_map<ParamName, Param> GetParams() const                   = 0;
    virtual GameObject& GetParentGameObject()                                        = 0;
    virtual const GameObject& getParentGameObject() const                            = 0;
    virtual void write(TreeNode&) const                                              = 0;
    virtual std::optional<IdType> getRegisteredFunctionId(FunctionType) const        = 0;
    virtual std::vector<FieldInfo> GetFields()                                       = 0;
};
}  // namespace Components
}  // namespace GameEngine
