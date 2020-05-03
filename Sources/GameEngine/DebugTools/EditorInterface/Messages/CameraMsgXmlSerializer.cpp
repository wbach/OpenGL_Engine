#include "CameraMsgXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const CameraMsg& input)
{
    auto root = std::make_unique<Utils::XmlNode>("CameraMsg");
    root->AddChild(Utils::Convert("position", input.position));
    root->AddChild(Utils::Convert("rotation", input.rotation));
    return root;
}
Network::IMessageData Serialize(const CameraMsg& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
