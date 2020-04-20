#include "ReloadSceneXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ReloadScene& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("filename"))
    {
        output.filename = input.attributes_.at("filename");
    }
}
std::unique_ptr<Network::IMessage> DeserializeReloadScene(Utils::XmlReader& reader)
{
    auto result = std::make_unique<ReloadScene>();
    auto msg = reader.Get("ReloadScene");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
