#include "LightDeletedXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(LightDeleted& output, TreeNode& input)
{
    if (input.isAttributePresent("id"))
    {
        output.id = std::stoi(input.attributes_.at("id"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeLightDeleted(Utils::XmlReader& reader)
{
    auto result = std::make_unique<LightDeleted>();
    auto msg = reader.Get("LightDeleted");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
