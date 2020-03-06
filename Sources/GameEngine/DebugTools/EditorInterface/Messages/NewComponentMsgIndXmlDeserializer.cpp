#include "NewComponentMsgIndXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewComponentMsgInd& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
    if (input.IsAttributePresent("isActive"))
    {
    }
}
std::unique_ptr<Network::IMessage> DeserializeNewComponentMsgInd(Utils::XmlReader& reader)
{
    auto result = std::make_unique<NewComponentMsgInd>();
    auto msg = reader.Get("NewComponentMsgInd");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
