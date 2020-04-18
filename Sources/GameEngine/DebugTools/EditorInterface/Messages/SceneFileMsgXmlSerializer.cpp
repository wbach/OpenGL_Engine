#include "SceneFileMsgXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const SceneFileMsg& input)
{
    auto root = std::make_unique<Utils::XmlNode>("SceneFileMsg");
    root->attributes_.insert({"filename", input.filename});
    return root;
}
Network::IMessageData Serialize(const SceneFileMsg& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
