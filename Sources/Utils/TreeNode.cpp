#include "TreeNode.h"

#include <sstream>

#include "Logger/Log.h"
#include "Rotation.h"
#include "Types.h"
#include "Utils/Utils.h"

TreeNode::TreeNode()
    : TreeNode("unknown")
{
}
TreeNode::TreeNode(const std::string& name)
    : parent(nullptr)
    , type_{"unknown"}
    , name_(name)
{
}

TreeNode::TreeNode(const std::string& name, const std::filesystem::path& path)
    : parent(nullptr)
    , value_(path.generic_string())
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
    children_.emplace_back(new TreeNode(name, value));
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
TreeNode& TreeNode::addChild(const TreeNode& child)
{
    children_.push_back(std::make_unique<TreeNode>(child));
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
    vec2 result{0.f};
    auto& attr = node.attributes_;

    auto itX = attr.find("x");
    auto itY = attr.find("y");

    try
    {
        if (itX != attr.end())
            result.x = std::stof(itX->second);
        if (itY != attr.end())
            result.y = std::stof(itY->second);
    }
    catch (...)
    {
        LOG_ERROR << "Read error (vec2): " << node;
    }

    return result;
}

vec3 ConvertToVec3(TreeNode& node)
{
    vec3 result{0.f};
    auto& attr = node.attributes_;

    auto itX = attr.find("x");
    auto itY = attr.find("y");
    auto itZ = attr.find("z");

    try
    {
        if (itX != attr.end())
            result.x = std::stof(itX->second);
        if (itY != attr.end())
            result.y = std::stof(itY->second);
        if (itZ != attr.end())
            result.z = std::stof(itZ->second);
    }
    catch (...)
    {
        LOG_ERROR << "Read error (vec3): " << node;
    }

    return result;
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
std::ostream& operator<<(std::ostream& os, const TreeNode& node)
{
    static int indent = 0;
    auto printIndent  = [&]()
    {
        for (int i = 0; i < indent; ++i)
            os << "  ";
    };

    printIndent();
    os << "[" << node.name() << "]";

    if (!node.value_.empty())
    {
        os << " value: \"" << node.value_ << "\"";
    }

    if (!node.attributes_.empty())
    {
        os << " (attrs: ";
        for (auto it = node.attributes_.begin(); it != node.attributes_.end(); ++it)
        {
            os << it->first << "=" << it->second << (std::next(it) != node.attributes_.end() ? ", " : "");
        }
        os << ")";
    }

    os << "\n";

    indent++;
    for (const auto& child : node.getChildren())
    {
        os << *child;
    }
    indent--;

    return os;
}
std::vector<const TreeNode*> TreeNode::getChildren(const std::string& name) const
{
    std::vector<const TreeNode*> result;

    for (const auto& child : children_)
    {
        if (child->name() == name)
        {
            result.push_back(child.get());
        }
    }
    return result;
}
