#pragma once
#include <memory>
#include <string>
#include <unordered_map>
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
        childrenMap_.insert({name, children_.back().get()});
        return *children_.back();
    }
    XmlNode& AddChild(const std::string& name, const std::string& value)
    {
        children_.emplace_back(new XmlNode(name, value));
        childrenMap_.insert({name, children_.back().get()});
        return *children_.back();
    }
    void AddChild(std::unique_ptr<XmlNode> child)
    {
        children_.push_back(std::move(child));
        auto ptr = children_.back().get();
        childrenMap_.insert({ptr->GetName(), ptr});
    }

    XmlNode* GetChild(const std::string& name) const
    {
        if (childrenMap_.count(name))
        {
            return childrenMap_.at(name);
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
    std::unordered_map<std::string, XmlNode*> childrenMap_;
    std::string name_;
};
}  // namespace Utils
