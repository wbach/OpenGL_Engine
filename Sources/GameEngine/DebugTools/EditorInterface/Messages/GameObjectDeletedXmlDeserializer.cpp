#include "GameObjectDeletedXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(GameObjectDeleted& output, TreeNode& input)
{
    if (input.isAttributePresent("gameObjectId"))
    {
        output.gameObjectId = std::stoi(input.attributes_.at("gameObjectId"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeGameObjectDeleted(Utils::XmlReader& reader)
{
    auto result = std::make_unique<GameObjectDeleted>();
    auto msg = reader.Get("GameObjectDeleted");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
