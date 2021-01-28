#include "ExitRespXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ExitResp& output, TreeNode& input)
{
    if (input.isAttributePresent("status"))
    {
        output.status = std::stoi(input.attributes_.at("status"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeExitResp(Utils::XmlReader& reader)
{
    auto result = std::make_unique<ExitResp>();
    auto msg = reader.Get("ExitResp");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
