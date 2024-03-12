#include "XmlMessageConverter.h"

#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>
#include <UtilsNetwork/MessageFormat.h>

#include "MessageTypes.h"

namespace common
{
namespace
{
std::string Convert(const std::vector<int8> &message)
{
    std::string result;
    std::copy(message.begin(), message.end(), std::back_inserter(result));
    return result;
}
}  // namespace

XmlMessageConverter::XmlMessageConverter()
{
}

bool XmlMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) and /*type >= COMMON_MESSAGE_TYPE_RANGE_LOW and*/
           type <= COMMON_MESSAGE_TYPE_RANGE_HIGH;
}

/*
    case common::MessageTypes::TransformReq = 0,
    case common::MessageTypes::TransformResp,
    case common::MessageTypes::DisconnectCharacter,
    case common::MessageTypes::SelectCharacterReq,
    case common::MessageTypes::SelectCharacterResp,
    case common::MessageTypes::CreateCharacterReq,
    case common::MessageTypes::CreateCharacterResp,
    case common::MessageTypes::GetCharactersReq,
    case common::MessageTypes::GetCharactersResp,
    case common::MessageTypes::GetCharacterDataReq,
    case common::MessageTypes::GetCharacterDataResp,
    case common::MessageTypes::GetCharactersDataReq
    */

std::unique_ptr<Network::IMessage> XmlMessageConverter::Convert(Network::IMessageType type,
                                                                const Network::IMessageData &message)
{
    std::string a = common::Convert(message);
    DEBUG_LOG(a);

    Utils::XmlReader reader;
    reader.ReadXml(a);

    switch (type)
    {
        case common::MessageTypes::TransformReq:
        {
        }
        break;
        case common::MessageTypes::TransformResp:
        {
        }
        break;
        case common::MessageTypes::DisconnectCharacter:
        {
        }
        break;
        case common::MessageTypes::SelectCharacterReq:
        {
        }
        break;
        case common::MessageTypes::SelectCharacterResp:
        {
        }
        break;
        case common::MessageTypes::CreateCharacterReq:
        {
        }
        break;
        case common::MessageTypes::CreateCharacterResp:
        {
        }
        break;
        case common::MessageTypes::GetCharactersReq:
        {
        }
        break;
        case common::MessageTypes::GetCharactersResp:
        {
        }
        break;
        case common::MessageTypes::GetCharacterDataReq:
        {
        }
        break;
        case common::MessageTypes::GetCharacterDataResp:
        {
        }
        break;
        case common::MessageTypes::GetCharactersDataReq:
        {
        }
        break;
        default:
            DEBUG_LOG("Convert to IMessage. Unsuporrted message.");
    }

    return nullptr;
}

Network::IMessageData XmlMessageConverter::Convert(const Network::IMessage &message)
{
    return {};
}

}  // namespace common
