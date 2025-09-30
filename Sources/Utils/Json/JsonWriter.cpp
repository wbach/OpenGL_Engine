#include "JsonWriter.h"

#include <Logger/Log.h>

#include <filesystem>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

namespace Utils
{
void WriteNode(const TreeNode& node, Value& outNode, Document::AllocatorType& allocator)
{
    bool hasContent = !node.attributes_.empty() || !node.getChildren().empty();

    if (hasContent)
    {
        outNode.SetObject();

        if (!node.attributes_.empty())
        {
            Value attrs(kObjectType);
            for (const auto& [key, val] : node.attributes_)
            {
                Value k(key.c_str(), static_cast<SizeType>(key.length()), allocator);
                Value v(val.c_str(), static_cast<SizeType>(val.length()), allocator);
                attrs.AddMember(k, v, allocator);
            }
            outNode.AddMember("attributes", attrs, allocator);
        }
        for (const auto& child : node.getChildren())
        {
            Value childVal(kObjectType);
            WriteNode(*child, childVal, allocator);
            Value k(child->name().c_str(), static_cast<SizeType>(child->name().length()), allocator);
            outNode.AddMember(k, childVal, allocator);
        }
    }
    else
    {
        // 2. Węzeł liść -> zapisujemy wartość bezpośrednio
        outNode.SetString(node.value_.c_str(), static_cast<SizeType>(node.value_.length()), allocator);
    }
}

std::string Json::Write(const TreeNode& root)
{
    Document doc;
    doc.SetObject();

    Value rootVal(kObjectType);
    WriteNode(root, rootVal, doc.GetAllocator());

    Value rootName(root.name().c_str(), static_cast<SizeType>(root.name().length()), doc.GetAllocator());
    doc.AddMember(rootName, rootVal, doc.GetAllocator());

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

void Json::Write(const std::string& filename, const TreeNode& root)
{
    auto parentPath = std::filesystem::path(filename).parent_path();
    if (!parentPath.empty() && !std::filesystem::exists(parentPath))
    {
        std::filesystem::create_directories(parentPath);
    }

    std::ofstream file(filename);
    if (!file.is_open())
    {
        LOG_ERROR << "cannot open file " << filename;
        return;
    }

    std::string jsonStr = Json::Write(root);
    file << jsonStr;
    file.close();
}

}  // namespace Utils
