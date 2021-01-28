#include "NewComponentMsgIndXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewComponentMsgInd& output, TreeNode& input)
{
    if (input.isAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
    if (input.isAttributePresent("isActive"))
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
