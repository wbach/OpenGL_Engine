#include "NewGameObjectIndXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewGameObjectInd& output, TreeNode& input)
{
    if (input.isAttributePresent("id"))
    {
        output.id = std::stoi(input.attributes_.at("id"));
    }
    if (input.isAttributePresent("parentId"))
    {
        output.parentId = std::stoi(input.attributes_.at("parentId"));
    }
    if (input.isAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
}
std::unique_ptr<Network::IMessage> DeserializeNewGameObjectInd(Utils::XmlReader& reader)
{
    auto result = std::make_unique<NewGameObjectInd>();
    auto msg = reader.Get("NewGameObjectInd");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
