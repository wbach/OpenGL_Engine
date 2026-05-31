#include "TreeNodeReadFunctions.h"

#include <charconv>
#include <string>

#include "Logger/Log.h"
#include "Utils.h"

template <typename T>
bool FastRead(const std::string& str, T& value)
{
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    return ec == std::errc();
}

void Read(const TreeNode& node, float& v)
{
    if (not FastRead(node.value_, v))
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

    if (not FastRead(node.value_, v))
    {
        LOG_ERROR << "read error " << node.value_;
    }
}

bool setIfExist(const TreeNode& node, const std::string& attributeName, float& v)
{
    auto attributeValueStr = node.getAttributeValue(attributeName);
    if (not attributeValueStr.empty())
    {
        if (FastRead(attributeValueStr, v))
        {
            return true;
        }
        else
        {
            LOG_ERROR << "read error " << node.value_;
        }
    }
    else
    {
        LOG_WARN << "Value not set";
    }

    return false;
}

bool setIfExist(const TreeNode& node, const std::string& attributeName, uint32& v)
{
    auto attributeValueStr = node.getAttributeValue(attributeName);
    if (not attributeValueStr.empty())
    {
        if (FastRead(node.value_, v))
        {
            return true;
        }
        else
        {
            LOG_ERROR << "read error " << node.value_;
        }
    }
    else
    {
        LOG_WARN << "Value not set";
    }

    return false;
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

void Read(const TreeNode& node, uint8& v)
{
    if (node.value_.empty() or not FastRead(node.value_, v))
    {
        LOG_ERROR << "Read error uint8: " << node.value_;
    }
}

void Read(const TreeNode& node, uint32& v)
{
    if (node.value_.empty())
    {
        LOG_WARN << "Value not set";
        return;
    }

    if (not FastRead(node.value_, v))
    {
        LOG_ERROR << "read error " << node.value_;
    }
}

void Read(const TreeNode& node, uint64& v)
{
    if (node.value_.empty())
    {
        LOG_WARN << "Value not set";
        return;
    }

    if (not FastRead(node.value_, v))
    {
        LOG_ERROR << "read error " << node.value_;
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
void Read(const TreeNode& node, std::vector<std::string>& result)
{
    for (const auto& node : node.getChildren())
    {
        std::string v;
        Read(*node, v);
        result.push_back(v);
    }
}
