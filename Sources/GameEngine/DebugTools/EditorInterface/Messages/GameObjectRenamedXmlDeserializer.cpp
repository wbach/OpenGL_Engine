#include "GameObjectRenamedXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(GameObjectRenamed& output, TreeNode& input)
{
    if (input.isAttributePresent("gameObjectId"))
    {
        output.gameObjectId = std::stoi(input.attributes_.at("gameObjectId"));
    }
    if (input.isAttributePresent("newName"))
    {
        output.newName = input.attributes_.at("newName");
    }
}
std::unique_ptr<Network::IMessage> DeserializeGameObjectRenamed(Utils::XmlReader& reader)
{
    auto result = std::make_unique<GameObjectRenamed>();
    auto msg = reader.Get("GameObjectRenamed");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
