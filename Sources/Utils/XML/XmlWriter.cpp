#include "XmlWriter.h"
#include <fstream>
#include "Logger/Log.h"
#include "Utils/Utils.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

using namespace rapidxml;

namespace Utils
{
class XmlNodeWrapper
{
public:
    XmlNodeWrapper(xml_document<>* doc, xml_node<>* node = nullptr, xml_node<>* root = nullptr)
        : node_(node)
        , root_(root)
        , document_(doc)
    {
    }
    XmlNodeWrapper& AddNode(const std::string& name, const std::string& value = std::string())
    {
        node_ = CreateNode(name, value);

        if (root_ == nullptr)
            document_->append_node(node_);
        else
            root_->append_node(node_);
        return *this;
    }
    XmlNodeWrapper& AddAtributte(const std::string& name, const std::string& value)
    {
        node_->append_attribute(document_->allocate_attribute(document_->allocate_string(name.c_str()),
                                                              document_->allocate_string(value.c_str())));

        return *this;
    }

    XmlNodeWrapper& AddChild(const std::string& name, const std::string& value = std::string())
    {
        auto child = CreateNode(name, value);
        node_->append_node(child);
        children_.emplace_back(document_, child, node_);
        return children_.back();
    }

private:
    xml_node<>* CreateNode(const std::string& name, const std::string& value = std::string())
    {
        xml_node<>* out = nullptr;

        if (value.empty())
            out = document_->allocate_node(node_element, document_->allocate_string(name.c_str()));
        else
            out = document_->allocate_node(node_element, document_->allocate_string(name.c_str()),
                                           document_->allocate_string(value.c_str()));

        return out;
    }

private:
    xml_node<>* node_;
    xml_node<>* root_;
    xml_document<>* document_;
    std::list<XmlNodeWrapper> children_;
};

class XmlCreator
{
public:
    XmlCreator()
    {
    }
    XmlNodeWrapper& CreateRoot(const std::string& name, const std::string& value = std::string())
    {
        root_ = std::make_unique<XmlNodeWrapper>(&document_, nullptr);
        return root_->AddNode(name, value);
    }

    void Save(const std::string& filename) const
    {
        std::ofstream file(filename.c_str());
        if (!file.is_open())
        {
            Error("cannot open file " + filename);
            return;
        }
        file << document_;
        file.close();
    }

private:
    xml_document<> document_;
    std::unique_ptr<XmlNodeWrapper> root_;
};

void WriteNode(XmlNode& node, XmlNodeWrapper& nodeWrapper);

void WroteNodeMembers(XmlNode& parent, XmlNodeWrapper& node)
{
    for (auto& attribute : parent.attributes_)
    {
        node = node.AddAtributte(attribute.first, attribute.second);
    }

    for (auto& c : parent.GetChildren())
    {
        WriteNode(*c, node);
    }
}

void WriteNode(XmlNode& node, XmlNodeWrapper& nodeWrapper)
{
    auto child = nodeWrapper.AddChild(node.GetName(), node.value_);
    WroteNodeMembers(node, child);
}

void Xml::Write(const std::string& filename, XmlNode& root)
{
    XmlCreator creator;
    auto& builderRoot = creator.CreateRoot(root.GetName(), root.value_);

    WroteNodeMembers(root, builderRoot);

    creator.Save(filename);
}
}  // namespace Utils
