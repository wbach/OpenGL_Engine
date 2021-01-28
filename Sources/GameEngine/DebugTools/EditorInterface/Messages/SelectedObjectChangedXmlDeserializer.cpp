#include "SelectedObjectChangedXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SelectedObjectChanged& output, TreeNode& input)
{
    if (input.isAttributePresent("id"))
    {
        output.id = std::stoi(input.attributes_.at("id"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeSelectedObjectChanged(Utils::XmlReader& reader)
{
    auto result = std::make_unique<SelectedObjectChanged>();
    auto msg = reader.Get("SelectedObjectChanged");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
