#include "ColladaCommon.h"
#include "GLM/GLMUtils.h"
#include "Logger/Log.h"
#include "Types.h"
#include "Utils.h"
#include "Utils/XML/XMLUtils.h"

namespace GameEngine
{
namespace Collada
{
void ProcessTechnique(XMLNode* root, TechniqueCommon& technique)
{
    Utils::ForEachSubNode(root, [&](const Utils::RapidNodeData& data, XMLNode* node) {
        if (data.name == "accessor")
        {
            ProcessAccessor(node, technique.accessor);
        }
    });
}
void ProcessAccessor(XMLNode* root, Accessor& accessor)
{
    auto source     = Utils::GetRapidAttributeData(root, "source");
    accessor.source = source.value;
    auto count      = Utils::GetRapidAttributeData(root, "count");
    accessor.count  = std::stoi(count.value);
    auto stride     = Utils::GetRapidAttributeData(root, "stride");
    accessor.stride = std::stoi(stride.value);

    Utils::ForEachSubNode(root, [&](const Utils::RapidNodeData& data, XMLNode* node) {
        if (data.name == "param")
        {
            accessor.params.emplace_back();
            ProcessParam(node, accessor.params.back());
        }
    });
}
void ProcessParam(XMLNode* root, Param& param)
{
    param.name = Utils::GetRapidAttributeData(root, "name").value;
    param.type = Utils::GetRapidAttributeData(root, "type").value;
}
void ProcessArray(XMLNode* root, Array& dataArray)
{
    auto id_data    = Utils::GetRapidAttributeData(root, "id");
    auto count_data = Utils::GetRapidAttributeData(root, "count");

    auto data = Utils::GetRapidNodeData(root);

    dataArray.count = std::stoi(count_data.value);
    dataArray.data  = data.value;
}
void ProcessSource(XMLNode* root, Source& source)
{
    auto aid = Utils::GetRapidAttributeData(root, "id");

    source.id = aid.value;

    Utils::ForEachSubNode(root, [&](const Utils::RapidNodeData& data, XMLNode* node) {
        if (data.name == "technique_common")
        {
            ProcessTechnique(node, source.technique);
        }
        if (data.name == "float_array")
        {
            source.dataArray.type = "float_array";
            ProcessArray(node, source.dataArray);
        }
        if (data.name == "Name_array")
        {
            source.dataArray.type = "Name_array";
            ProcessArray(node, source.dataArray);
        }
    });
}
void ProcessInput(XMLNode* root, Input& input)
{
    input.semantic = Utils::GetRapidAttributeData(root, "semantic").value;
    input.sourceId = Utils::GetRapidAttributeData(root, "source").value;

    auto soffset = Utils::GetRapidAttributeData(root, "offset").value;
    if (!soffset.empty())
        input.offset = std::stoi(soffset);
}
vec4 GetVector4FromString(const std::string& str)
{
    auto strs = Utils::SplitString(str, ' ');

    FloatVector tmp;

    for (const auto& str : strs)
    {
        auto a = Utils::StringToFloat(str);
        tmp.push_back(a);

        if (tmp.size() >= 4)
        {
            return vec4(tmp[0], tmp[1], tmp[2], tmp[3]);
        }
    }
    return vec4();
}
mat4 GetMatrixFromString(const std::string& str)
{
    auto mdata = GetFloatsFromString(str);

    mat4 transform_matrix(1.f);
    if (mdata.size() != 16)
    {
        DEBUG_LOG("GetMatrixFromString string is wrong size.");
        return transform_matrix;
    }

    uint16 i = 0;
    for (uint16 y = 0; y < 4; ++y)
        for (uint16 x              = 0; x < 4; ++x)
            transform_matrix[x][y] = mdata[i++];

    return transform_matrix;
}
Mat4Vector GetMatrixesFromString(const std::string& str)
{
    Mat4Vector output;
    auto mdata = GetFloatsFromString(str);
    output.reserve(mdata.size() / 16);
    mat4 transform_matrix(1.f);
    int y = 0, x = 0;
    for (const auto& f : mdata)
    {
        transform_matrix[x][y] = f;
        ++x;
        if (x >= 4)
        {
            x = 0;
            ++y;
        }

        if (y >= 4)
        {
            output.push_back(transform_matrix);
            y = 0;
            x = 0;
        }
    }

    return output;
}
Vec2Vector GetVectors2dFromString(const std::string& str)
{
    Vec2Vector out;
    GetVectors2dFromString(str, out);
    return out;
}
vec3 GetVector3(const vec4& v)
{
    return vec3(v.x, v.y, v.z);
}
void GetVectors2dFromString(const std::string& str, Vec2Vector& vv)
{
    auto strs = Utils::SplitString(str, ' ');
    vv.reserve(strs.size());
    FloatVector tmp;

    for (const auto& str : strs)
    {
        auto a = Utils::StringToFloat(str);
        tmp.push_back(a);

        if (tmp.size() >= 2)
        {
            vec2 v(tmp[0], tmp[1]);
            vv.push_back(v);
            tmp.clear();
        }
    }
}
Vec3Vector GetVectors3dFromString(const std::string& str)
{
    Vec3Vector out;
    GetVectors3dFromString(str, out);
    return out;
}
void GetVectors3dFromString(const std::string& str, Vec3Vector& vert)
{
    auto strs = Utils::SplitString(str, ' ');
    vert.reserve(strs.size());
    FloatVector tmp;

    for (const auto& str : strs)
    {
        auto a = Utils::StringToFloat(str);
        tmp.push_back(a);

        if (tmp.size() >= 3)
        {
            vec3 v(tmp[0], tmp[1], tmp[2]);
            vert.push_back(v);
            tmp.clear();
        }
    }
}
FloatVector GetFloatsFromString(const std::string& str)
{
    FloatVector out;

    GetFloatsFromString(str, out);

    return out;
}
void GetFloatsFromString(const std::string& str, FloatVector& v)
{
    auto strs = Utils::SplitString(str, ' ');
    v.reserve(str.size());

    for (const auto& str : strs)
    {
        auto a = Utils::StringToFloat(str);
        v.push_back(a);
    }
}
Uint32Vector GetIntsFromString(const std::string& str)
{
    Uint32Vector out;
    auto strs = Utils::SplitString(str, ' ');
    out.reserve(strs.size());
    for (const auto& str : strs)
    {
        auto a = Utils::StringToInt(str);
        out.push_back(a);
    }
    return out;
}
StrVector GetStringsFromString(const std::string& str)
{
    return Utils::SplitString(str, ' ');
}
std::string GetSource(const std::string& str)
{
    if (str.empty())
        return {};

    if (str[0] == '#')
        return str.substr(1);

    return str;
}
std::string GetName(const std::string& str, std::string& postfix)
{
    return str.substr(0, str.find(postfix));
}
}  // Collada
}  // GameEngine
