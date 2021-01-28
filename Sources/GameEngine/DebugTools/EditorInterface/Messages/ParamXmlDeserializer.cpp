#include "ParamXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(Param& output, TreeNode& input)
{
    if (input.isAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
    if (input.isAttributePresent("value"))
    {
        output.value = input.attributes_.at("value");
    }
    if (input.isAttributePresent("type"))
    {
        output.type = input.attributes_.at("type");
    }
}
std::unique_ptr<Network::IMessage> DeserializeParam(Utils::XmlReader& reader)
{
    auto result = std::make_unique<Param>();
    auto msg = reader.Get("Param");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
