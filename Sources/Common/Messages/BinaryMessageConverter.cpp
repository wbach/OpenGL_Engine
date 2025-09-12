#include "BinaryMessageConverter.h"

#include <Common/Messages/CreateCharacter/CreateCharacterMsgReq.h>
#include <Common/Messages/CreateCharacter/CreateCharacterMsgResp.h>
#include <Common/Messages/GetCharacterData/GetCharacterDataMsgReq.h>
#include <Common/Messages/GetCharacterData/GetCharacterDataMsgResp.h>
#include <Common/Messages/GetCharacterData/GetCharactersDataMsgReq.h>
#include <Common/Messages/GetCharacters/GetCharactersMsgReq.h>
#include <Common/Messages/GetCharacters/GetCharactersMsgResp.h>
#include <Common/Messages/MessageTypes.h>
#include <Common/Messages/RemoveCharacter/DisconnectCharacterMsg.h>
#include <Common/Messages/SelectCharacter/SelectCharacterMsgReq.h>
#include <Common/Messages/SelectCharacter/SelectCharacterMsgResp.h>
#include <Common/Messages/TransformMessages/TransformMsgReq.h>
#include <Common/Messages/TransformMessages/TransformMsgResp.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/PayloadUtils.h>

namespace common
{
BinaryMessageConverter::BinaryMessageConverter()
{
}

bool BinaryMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Binary) and type >= COMMON_MESSAGE_TYPE_RANGE_LOW and
           type <= COMMON_MESSAGE_TYPE_RANGE_HIGH;
}

std::unique_ptr<Network::IMessage> BinaryMessageConverter::Convert(Network::IMessageType type,
                                                                   const Network::IMessageData& message)
{
    switch (type)
    {
        case MessageTypes::TransformReq:
            return Network::ConvertMessage<common::TransformMsgReq>(message);
        case MessageTypes::TransformResp:
            return Network::ConvertMessage<TransformMsgResp>(message);
        case MessageTypes::DisconnectCharacter:
            return Network::ConvertMessage<DisconnectCharacterMsg>(message);
        case MessageTypes::SelectCharacterReq:
            return Network::ConvertMessage<SelectCharacterMsgReq>(message);
        case MessageTypes::SelectCharacterResp:
            return Network::ConvertMessage<SelectCharacterMsgResp>(message);
        case MessageTypes::CreateCharacterReq:
            return Network::ConvertMessage<CreateCharacterMsgReq>(message);
        case MessageTypes::CreateCharacterResp:
            return Network::ConvertMessage<CreateCharacterMsgResp>(message);
        case MessageTypes::GetCharactersReq:
            return Network::ConvertMessage<GetCharactersMsgReq>(message);
        case MessageTypes::GetCharactersResp:
            return Network::ConvertMessage<GetCharactersMsgResp>(message);
        case MessageTypes::GetCharacterDataReq:
            return Network::ConvertMessage<GetCharacterDataMsgReq>(message);
        case MessageTypes::GetCharacterDataResp:
            return Network::ConvertMessage<GetCharacterDataMsgResp>(message);
        case MessageTypes::GetCharactersDataReq:
            return Network::ConvertMessage<GetCharactersDataMsgReq>(message);
        default:
            LOG_DEBUG << "Convert to IMessage. Unsuporrted message.";
    }

    return nullptr;
}

Network::IMessageData BinaryMessageConverter::Convert(const Network::IMessage& message)
{
    switch (message.GetType())
    {
        case MessageTypes::TransformReq:
            return Network::ConvertMessage<common::TransformMsgReq>(message);
        case MessageTypes::TransformResp:
            return Network::ConvertMessage<TransformMsgResp>(message);
        case MessageTypes::DisconnectCharacter:
            return Network::ConvertMessage<DisconnectCharacterMsg>(message);
        case MessageTypes::SelectCharacterReq:
            return Network::ConvertMessage<SelectCharacterMsgReq>(message);
        case MessageTypes::SelectCharacterResp:
            return Network::ConvertMessage<SelectCharacterMsgResp>(message);
        case MessageTypes::CreateCharacterReq:
            return Network::ConvertMessage<CreateCharacterMsgReq>(message);
        case MessageTypes::CreateCharacterResp:
            return Network::ConvertMessage<CreateCharacterMsgResp>(message);
        case MessageTypes::GetCharactersReq:
            return Network::ConvertMessage<GetCharactersMsgReq>(message);
        case MessageTypes::GetCharactersResp:
            return Network::ConvertMessage<GetCharactersMsgResp>(message);
        case MessageTypes::GetCharacterDataReq:
            return Network::ConvertMessage<GetCharacterDataMsgReq>(message);
        case MessageTypes::GetCharacterDataResp:
            return Network::ConvertMessage<GetCharacterDataMsgResp>(message);
        case MessageTypes::GetCharactersDataReq:
            return Network::ConvertMessage<GetCharactersDataMsgReq>(message);
        default:
            LOG_DEBUG << "Convert to IMessage. Unsuporrted message.";
    }

    LOG_DEBUG << "Convert to binary. Unsuporrted message.";
    return {};
}
}  // namespace common
