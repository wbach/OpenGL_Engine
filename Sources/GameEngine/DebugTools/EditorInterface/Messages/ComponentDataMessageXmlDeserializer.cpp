#include "ComponentDataMessageXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>
#include "ParamXmlDeserializer.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ComponentDataMessage& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
    if (input.IsAttributePresent("gameObjectId"))
    {
        output.gameObjectId = std::stoi(input.attributes_.at("gameObjectId"));
    }
    if (input.IsAttributePresent("params"))
    {
        for (auto& child : input.GetChild("params")->GetChildren())
        {
            Param obj;
            SetParam(obj, *child);
            output.params.push_back(obj);
        }
    }
    if (input.IsAttributePresent("testVec3Param"))
    {
        output.testVec3Param = Utils::ConvertToVec3(*input.GetChild("testVec3Param"));
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
