#include "TreeNode.h"

#include <sstream>

#include "Logger/Log.h"
#include "Utils/Utils.h"

namespace
{
const std::string CSTR_X = "x";
const std::string CSTR_Y = "y";
const std::string CSTR_Z = "z";
const std::string CSTR_W = "w";
}  // namespace

TreeNode::TreeNode(const std::string& name)
    : parent(nullptr)
    , type_{"unknown"}
    , name_(name)
{
}
TreeNode::TreeNode(const std::string& name, const std::string& value)
    : parent(nullptr)
    , value_(value)
    , type_{"unknown"}
    , name_(name)
{
}

TreeNode::TreeNode(const TreeNode& source)
{
    CopyTreeNode(source, *this);
}

TreeNode& TreeNode::operator=(const TreeNode& source)
{
    if (this == &source)
        return *this;

    CopyTreeNode(source, *this);
    return *this;
}
const std::string& TreeNode::name() const
{
    return name_;
}
const std::vector<std::unique_ptr<TreeNode>>& TreeNode::getChildren() const
{
    return children_;
}
TreeNode& TreeNode::addChild(const std::string& name)
{
    children_.emplace_back(new TreeNode(name));
    return *children_.back();
}
TreeNode& TreeNode::addChild(const std::string& name, const std::string& value)
{
    children_.emplace_back(new TreeNode(name, value));
    return *children_.back();
}

TreeNode& TreeNode::addChild(const std::string& name, const std::filesystem::path& value)
{
    children_.emplace_back(new TreeNode(name, Utils::ReplaceSlash(value.string())));
    return *children_.back();
}

TreeNode& TreeNode::addChild(const std::string& name, const std::string_view& value)
{
    children_.emplace_back(new TreeNode(name, std::string(value)));
    return *children_.back();
}
TreeNode& TreeNode::addChild(std::unique_ptr<TreeNode> child)
{
    children_.push_back(std::move(child));
    return *children_.back();
}
TreeNode* TreeNode::getChild(const std::string& name) const
{
    auto childIter =
        std::find_if(children_.begin(), children_.end(), [&name](const auto& child) { return child->name() == name; });
    if (childIter != children_.end())
    {
        return childIter->get();
    }
    return nullptr;
}
bool TreeNode::isAttributePresent(const std::string& name) const
{
    return attributes_.count(name) > 0;
}
std::string TreeNode::getAttributeValue(const std::string& attributeName) const
{
    auto iter = attributes_.find(attributeName);
    return iter != attributes_.end() ? iter->second : std::string();
}

const std::string CSTR_VEC3 = "vec3";

void Read(const TreeNode& node, float& v)
{
    try
    {
        v = std::stof(node.value_);
    }
    catch (...)
    {
        LOG_ERROR << "read error " << node.value_;
    }
}

void Read(const TreeNode& node, bool& v)
{
    v = Utils::StringToBool(node.value_);
}

void Read(const TreeNode& node, int32& v)
{
    v = std::stoi(node.value_);
}

void setIfExist(const TreeNode& node, const std::string& attributeName, float& v)
{
    auto attributeValueStr = node.getAttributeValue(attributeName);
    if (not attributeValueStr.empty())
    {
        v = std::stof(attributeValueStr);
    }
}

void setIfExist(const TreeNode& node, const std::string& attributeName, uint32& v)
{
    auto attributeValueStr = node.getAttributeValue(attributeName);
    if (not attributeValueStr.empty())
    {
        v = std::stoi(attributeValueStr);
    }
}

void Read(const TreeNode& node, vec3& v)
{
    try
    {
        setIfExist(node, CSTR_X, v.x);
        setIfExist(node, CSTR_Y, v.y);
        setIfExist(node, CSTR_Z, v.z);
    }
    catch (...)
    {
        LOG_ERROR << "Read error";
    }
}
void Read(const TreeNode& node, vec4& v)
{
    try
    {
        setIfExist(node, CSTR_X, v.x);
        setIfExist(node, CSTR_Y, v.y);
        setIfExist(node, CSTR_Z, v.z);
        setIfExist(node, CSTR_W, v.w);
    }
    catch (...)
    {
        LOG_ERROR << "Read error";
    }
}

void Read(const TreeNode& node, Quaternion& v)
{
    try
    {
        setIfExist(node, CSTR_X, v.x);
        setIfExist(node, CSTR_Y, v.y);
        setIfExist(node, CSTR_Z, v.z);
        setIfExist(node, CSTR_W, v.w);
    }
    catch (...)
    {
        LOG_ERROR << "Read error";
    }
}

void Read(const TreeNode& node, std::string& v)
{
    v = node.value_;
}

void Read(const TreeNode& node, std::vector<vec2>& result)
{
    for (const auto& node : node.getChildren())
    {
        vec2 v(0);
        Read(*node, v);
        result.push_back(v);
    }
}

void Read(const TreeNode& node, std::vector<vec3>& result)
{
    for (const auto& node : node.getChildren())
    {
        vec3 v(0);
        Read(*node, v);
        result.push_back(v);
    }
}

void Read(const TreeNode& node, vec2ui& v)
{
    setIfExist(node, CSTR_X, v.x);
    setIfExist(node, CSTR_Y, v.y);
}

void Read(const TreeNode& node, uint32& v)
{
    v = std::stoi(node.value_);
}

void Read(const TreeNode& node, vec2& v)
{
    setIfExist(node, CSTR_X, v.x);
    setIfExist(node, CSTR_Y, v.y);
}

std::unique_ptr<TreeNode> Convert(const std::string& v)
{
    return std::make_unique<TreeNode>("v", v);
}

std::unique_ptr<TreeNode> Convert(const std::string& label, const vec2& v)
{
    // clang-format off
    auto root = std::make_unique<TreeNode>(label);
    root->attributes_ =
    {
        { CSTR_X, std::to_string(v.x) },
        { CSTR_Y, std::to_string(v.y) }
    };
    // clang-format on
    return root;
}

std::unique_ptr<TreeNode> Convert(const std::string& label, const vec3& v)
{
    // clang-format off
    auto root = std::make_unique<TreeNode>(label);
    root->attributes_ =
    {
        { CSTR_X, std::to_string(v.x) },
        { CSTR_Y, std::to_string(v.y) },
        { CSTR_Z, std::to_string(v.z) }
    };
    // clang-format on
    return root;
}

std::unique_ptr<TreeNode> Convert(const std::string& label, const Quaternion& v)
{
    auto root = std::make_unique<TreeNode>(label);
    // clang-format off
    root->attributes_ =
    {
        { CSTR_X, std::to_string(v.x) },
        { CSTR_Y, std::to_string(v.y) },
        { CSTR_Z, std::to_string(v.z) },
        { CSTR_W, std::to_string(v.w) }
    };
    // clang-format on
    return root;
}

vec2 ConvertToVec2(TreeNode& node)
{
    vec2 result;
    result.x = std::stof(node.attributes_.at("x"));
    result.y = std::stof(node.attributes_.at("y"));
    return result;
}

vec3 ConvertToVec3(TreeNode& node)
{
    vec3 result;
    result.x = std::stof(node.attributes_.at("x"));
    result.y = std::stof(node.attributes_.at("y"));
    result.z = std::stof(node.attributes_.at("z"));
    return result;
}

void write(TreeNode& node, float v)
{
    node.value_ = std::to_string(v);
}
void write(TreeNode& node, int v)
{
    node.value_ = std::to_string(v);
}
void write(TreeNode& node, uint32 v)
{
    node.value_ = std::to_string(v);
}
void write(TreeNode& node, bool b)
{
    node.value_ = Utils::BoolToString(b);
}
void write(TreeNode& node, const std::string& str)
{
    node.value_ = str;
}
void write(TreeNode& node, const std::string_view& str)
{
    node.value_ = str;
}
void write(TreeNode& node, const std::filesystem::path& value)
{
    node.value_ = Utils::ReplaceSlash(value.string());
}
void write(TreeNode& node, const vec2ui& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
}
void write(TreeNode& node, const vec2& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
}
void write(TreeNode& node, const vec3& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
    node.attributes_[CSTR_Z] = std::to_string(v.z);
}
void write(TreeNode& node, const vec4& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
    node.attributes_[CSTR_Z] = std::to_string(v.z);
    node.attributes_[CSTR_W] = std::to_string(v.w);
}
void write(TreeNode& node, const std::optional<uint32>& v)
{
    if (v)
        write(node, *v);
    else
        node.value_ = "-";
}
void write(TreeNode& node, const std::vector<vec3>& v)
{
    for (const auto& value : v)
    {
        ::write(node.addChild(CSTR_VEC3), value);
    }
}

void TreeNode::CopyTreeNode(const TreeNode& source, TreeNode& target)
{
    target.parent      = source.parent;
    target.value_      = source.value_;
    target.type_       = source.type_;
    target.attributes_ = source.attributes_;
    target.name_       = source.name_;

    for (const auto& sourceChild : source.children_)
    {
        auto& targetChild = target.addChild(sourceChild->name_);
        CopyTreeNode(*sourceChild, targetChild);
    }
}

void PrintTree(const TreeNode& root)
{
    std::function<void(const TreeNode&, int)> printRec;

    printRec = [&](const TreeNode& node, int indent)
    {
        std::string pad(indent * 2, ' ');
        std::stringstream ss;
        ss << pad << "- " << node.name();

        if (!node.value_.empty())
        {
            ss << " = " << node.value_;
        }

        LOG_DEBUG << ss.str();

        // Atrybuty
        for (const auto& [k, v] : node.attributes_)
        {
            LOG_DEBUG << pad << "   @" << k << " = " << v << "\n";
        }

        // Dzieci
        for (const auto& child : node.getChildren())
        {
            printRec(*child, indent + 1);
        }
    };

    printRec(root, 0);
}