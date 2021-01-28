#include "SceneStopedNotifMsgXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SceneStopedNotifMsg& output, TreeNode& input)
{
    if (input.isAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
}
std::unique_ptr<Network::IMessage> DeserializeSceneStopedNotifMsg(Utils::XmlReader& reader)
{
    auto result = std::make_unique<SceneStopedNotifMsg>();
    auto msg = reader.Get("SceneStopedNotifMsg");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
