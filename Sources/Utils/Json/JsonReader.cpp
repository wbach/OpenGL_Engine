#include "JsonReader.h"

#include <string>

#ifdef __GNUC__  // GCC i Clang
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <algorithm>
#include <fstream>
#include <sstream>

#include "Logger/Log.h"
#include "Utils/FileSystem/FileSystemUtils.hpp"

using namespace rapidjson;

namespace
{
void ParseNode(const rapidjson::Value& value, TreeNode& node)
{
    if (value.IsObject())
    {
        for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
        {
            std::string name          = it->name.GetString();
            const rapidjson::Value& v = it->value;

            if (name == "attributes" && v.IsObject())
            {
                // wypełniamy attributes_ scalarnymi wartościami
                for (auto att = v.MemberBegin(); att != v.MemberEnd(); ++att)
                {
                    const auto& attValue = att->value;
                    std::string valStr;
                    if (attValue.IsString())
                        valStr = attValue.GetString();
                    else if (attValue.IsInt())
                        valStr = std::to_string(attValue.GetInt());
                    else if (attValue.IsUint())
                        valStr = std::to_string(attValue.GetUint());
                    else if (attValue.IsDouble())
                        valStr = std::to_string(attValue.GetDouble());
                    else if (attValue.IsBool())
                        valStr = attValue.GetBool() ? "true" : "false";

                    node.attributes_[att->name.GetString()] = valStr;
                }
            }
            else
            {
                // tworzymy dziecko
                auto& child = node.addChild(name);

                // jeśli scalar, ustawiamy value_
                if (v.IsString())
                    child.value_ = v.GetString();
                else if (v.IsInt())
                    child.value_ = std::to_string(v.GetInt());
                else if (v.IsUint())
                    child.value_ = std::to_string(v.GetUint());
                else if (v.IsDouble())
                    child.value_ = std::to_string(v.GetDouble());
                else if (v.IsBool())
                    child.value_ = v.GetBool() ? "true" : "false";
                else if (v.IsObject() || v.IsArray())
                    ParseNode(v, child);

                child.parent = &node;
            }
        }
    }
    else if (value.IsArray())
    {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
        {
            auto& child   = node.addChild(std::to_string(i));
            const auto& v = value[i];

            if (v.IsString())
                child.value_ = v.GetString();
            else if (v.IsInt())
                child.value_ = std::to_string(v.GetInt());
            else if (v.IsUint())
                child.value_ = std::to_string(v.GetUint());
            else if (v.IsDouble())
                child.value_ = std::to_string(v.GetDouble());
            else if (v.IsBool())
                child.value_ = v.GetBool() ? "true" : "false";
            else
                ParseNode(v, child);

            child.parent = &node;
        }
    }
}
}  // namespace
namespace Utils
{
bool JsonReader::Read(const std::filesystem::path& filename)
{
    try
    {
        auto str = Utils::ReadFile(filename);
        if (str.empty())
        {
            LOG_ERROR << "File empty. " << filename;
            return false;
        }

        return ReadJson(str);
    }
    catch (...)
    {
        LOG_ERROR << "Json read error! filename=" << filename;
    }
    return false;
}

bool JsonReader::ReadJson(const std::string& fileContent)
{
    Document document;
    ParseResult ok = document.Parse(fileContent.c_str());

    if (!ok)
    {
        LOG_ERROR << "JSON parse error: " << GetParseError_En(ok.Code()) << " (offset " << ok.Offset() << ")";
        LOG_ERROR << fileContent;
        return false;
    }

    root_ = std::make_unique<TreeNode>("root");
    ParseNode(document, *root_);

    return true;
}

TreeNode* JsonReader::Get()
{
    return root_.get();
}

TreeNode* JsonReader::Get(const std::string& name, TreeNode* node)
{
    if (node == nullptr)
        node = root_.get();

    if (node->name() == name)
        return node;

    auto childIter = std::find_if(node->getChildren().begin(), node->getChildren().end(),
                                  [&name](const auto& child) { return (child->name() == name); });

    if (childIter != node->getChildren().end())
    {
        return childIter->get();
    }

    for (const auto& child : node->getChildren())
    {
        auto n = Get(name, child.get());
        if (n)
        {
            return n;
        }
    }

    return nullptr;
}
}  // namespace Utils
