#include "TreeNodeReadFunctions.h"

#include "Logger/Log.h"
#include "Utils.h"

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
    if (node.value_.empty())
    {
        LOG_WARN << "Value not set";
        return;
    }

    try
    {
        v = std::stoi(node.value_);
    }
    catch (...)
    {
        LOG_ERROR << "Read error: " << node;
    }
}

bool setIfExist(const TreeNode& node, const std::string& attributeName, float& v)
{
    auto attributeValueStr = node.getAttributeValue(attributeName);
    if (not attributeValueStr.empty())
    {
        try
        {
            v = std::stof(attributeValueStr);
        }
        catch (...)
        {
            LOG_ERROR << "Read error: " << node;
        }
        return true;
    }
    else
    {
        LOG_WARN << "Value not set";
    }

    return false;
}

void setIfExist(const TreeNode& node, const std::string& attributeName, uint32& v)
{
    auto attributeValueStr = node.getAttributeValue(attributeName);
    if (not attributeValueStr.empty())
    {
        try
        {
            v = std::stoi(attributeValueStr);
        }
        catch (...)
        {
            LOG_ERROR << "Stoi error";
        }
    }
    else
    {
        LOG_WARN << "Value not set";
        return;
    }
}

void Read(const TreeNode& node, vec3& v)
{
    try
    {
        bool is = setIfExist(node, CSTR_X, v.x) and setIfExist(node, CSTR_Y, v.y) and setIfExist(node, CSTR_Z, v.z);
        if (not is)
        {
            std::from_string(node.value_, v);
        }
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
        bool is = setIfExist(node, CSTR_X, v.x) and setIfExist(node, CSTR_Y, v.y) and setIfExist(node, CSTR_Z, v.z) and
                  setIfExist(node, CSTR_W, v.w);

        if (not is)
        {
            std::from_string(node.value_, v);
        }
    }
    catch (...)
    {
        LOG_ERROR << "Read error";
    }
}

void Read(const TreeNode& node, Color& color)
{
    Read(node, color.value);
}

void Read(const TreeNode& node, Quaternion& v)
{
    try
    {
        bool is = setIfExist(node, CSTR_X, v.x) and setIfExist(node, CSTR_Y, v.y) and setIfExist(node, CSTR_Z, v.z) and
                  setIfExist(node, CSTR_W, v.w);

        if (not is)
        {
            std::from_string(node.value_, v);
        }
    }
    catch (...)
    {
        LOG_ERROR << "Read error";
    }
}

void Read(const TreeNode& node, Rotation& v)
{
    Read(node, v.value_);
}

void Read(const TreeNode& node, std::string& v)
{
    v = node.value_;
}
void Read(const TreeNode& node, std::filesystem::path& v)
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
    if (node.value_.empty())
    {
        LOG_WARN << "Value not set";
        return;
    }

    try
    {
        v = std::stoi(node.value_);
    }
    catch (...)
    {
        LOG_ERROR << "Read error: " << node;
    }
}

void Read(const TreeNode& node, vec2& v)
{
    try
    {
        bool is = setIfExist(node, CSTR_X, v.x) and setIfExist(node, CSTR_Y, v.y);

        if (not is)
        {
            std::from_string(node.value_, v);
        }
    }
    catch (...)
    {
        LOG_ERROR << "Read error: " << node;
    }
}
