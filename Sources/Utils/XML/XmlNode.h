#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <vector>

namespace Utils
{
typedef std::unordered_map<std::string, std::string> Attributes;

class XmlNode
{
public:
    XmlNode(const std::string& name)
        : parent(nullptr)
        , name_(name)
    {
    }
    XmlNode(const std::string& name, const std::string& value)
        : parent(nullptr)
        , value_(value)
        , name_(name)
    {
    }

    const std::string& GetName() const
    {
        return name_;
    }
    const std::vector<std::unique_ptr<XmlNode>>& GetChildren() const
    {
        return children_;
    }
    XmlNode& AddChild(const std::string& name)
    {
        children_.emplace_back(new XmlNode(name));
        return *children_.back();
    }
    XmlNode& AddChild(const std::string& name, const std::string& value)
    {
        children_.emplace_back(new XmlNode(name, value));
        return *children_.back();
    }
    void AddChild(std::unique_ptr<XmlNode> child)
    {
        children_.push_back(std::move(child));
    }

    XmlNode* GetChild(const std::string& name) const
    {
        auto childIter = std::find_if(children_.begin(), children_.end(), [&name](const auto& child) {
            return child->GetName() == name;
        });
        if (childIter != children_.end())
        {
            return childIter->get();
        }
        return nullptr;
    }

    bool IsAttributePresent(const std::string& name) const
    {
        return attributes_.count(name) > 0;
    }

public:
    XmlNode* parent;
    std::string value_;
    Attributes attributes_;

private:
    std::vector<std::unique_ptr<XmlNode>> children_;
    std::string name_;
};
}  // namespace Utils
