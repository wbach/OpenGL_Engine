#include "RemoveGameObjectIndXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(RemoveGameObjectInd& output, TreeNode& input)
{
    if (input.isAttributePresent("id"))
    {
        output.id = std::stoi(input.attributes_.at("id"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeRemoveGameObjectInd(Utils::XmlReader& reader)
{
    auto result = std::make_unique<RemoveGameObjectInd>();
    auto msg = reader.Get("RemoveGameObjectInd");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
