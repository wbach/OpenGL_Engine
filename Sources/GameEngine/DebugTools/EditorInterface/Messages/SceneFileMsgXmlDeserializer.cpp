#include "SceneFileMsgXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SceneFileMsg& output, TreeNode& input)
{
    if (input.isAttributePresent("filename"))
    {
        output.filename = input.attributes_.at("filename");
    }
}
std::unique_ptr<Network::IMessage> DeserializeSceneFileMsg(Utils::XmlReader& reader)
{
    auto result = std::make_unique<SceneFileMsg>();
    auto msg = reader.Get("SceneFileMsg");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
