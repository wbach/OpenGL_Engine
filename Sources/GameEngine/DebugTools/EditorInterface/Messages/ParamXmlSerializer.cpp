#include "ParamXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const Param& input)
{
    auto root = std::make_unique<Utils::XmlNode>("Param");
    root->attributes_.insert({"name", input.name});
    root->attributes_.insert({"value", input.value});
    root->attributes_.insert({"type", input.type});
    return root;
}
Network::IMessageData Serialize(const Param& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
