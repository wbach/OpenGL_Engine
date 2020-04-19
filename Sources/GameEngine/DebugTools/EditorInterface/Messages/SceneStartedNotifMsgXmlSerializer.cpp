#include "SceneStartedNotifMsgXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const SceneStartedNotifMsg& input)
{
    auto root = std::make_unique<Utils::XmlNode>("SceneStartedNotifMsg");
    root->attributes_.insert({"name", input.name});
    return root;
}
Network::IMessageData Serialize(const SceneStartedNotifMsg& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
