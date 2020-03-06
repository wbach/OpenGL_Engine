#include "NewGameObjectIndXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewGameObjectInd& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("id"))
    {
        output.id = std::stoi(input.attributes_.at("id"));
    }
    if (input.IsAttributePresent("parentId"))
    {
        output.parentId = std::stoi(input.attributes_.at("parentId"));
    }
    if (input.IsAttributePresent("name"))
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
