#include "SceneStartedNotifMsgXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SceneStartedNotifMsg& output, TreeNode& input)
{
    if (input.isAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
}
std::unique_ptr<Network::IMessage> DeserializeSceneStartedNotifMsg(Utils::XmlReader& reader)
{
    auto result = std::make_unique<SceneStartedNotifMsg>();
    auto msg = reader.Get("SceneStartedNotifMsg");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
