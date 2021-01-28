#include "ComponentDataMessageXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>
#include "ParamXmlDeserializer.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ComponentDataMessage& output, TreeNode& input)
{
    if (input.isAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
    if (input.isAttributePresent("gameObjectId"))
    {
        output.gameObjectId = std::stoi(input.attributes_.at("gameObjectId"));
    }
    if (input.isAttributePresent("params"))
    {
        for (auto& child : input.getChild("params")->getChildren())
        {
            Param obj;
            SetParam(obj, *child);
            output.params.push_back(obj);
        }
    }
}
std::unique_ptr<Network::IMessage> DeserializeComponentDataMessage(Utils::XmlReader& reader)
{
    auto result = std::make_unique<ComponentDataMessage>();
    auto msg = reader.Get("ComponentDataMessage");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
