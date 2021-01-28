#include "ComponentDeletedXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ComponentDeleted& output, TreeNode& input)
{
    if (input.isAttributePresent("gameObjectId"))
    {
        output.gameObjectId = std::stoi(input.attributes_.at("gameObjectId"));
    }
    if (input.isAttributePresent("componentName"))
    {
        output.componentName = input.attributes_.at("componentName");
    }
}
std::unique_ptr<Network::IMessage> DeserializeComponentDeleted(Utils::XmlReader& reader)
{
    auto result = std::make_unique<ComponentDeleted>();
    auto msg = reader.Get("ComponentDeleted");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
