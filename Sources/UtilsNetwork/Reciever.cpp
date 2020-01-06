#include "Reciever.h"

#include "GLM/GLMUtils.h"
#include "Logger/Log.h"
#include "Messages/Conntection/AuthenticationMessage.h"
#include "Messages/Conntection/ConnectionMessage.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgReq.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgResp.h"
#include "Messages/GetCharacterData/GetCharactersDataMsgReq.h"
#include "Messages/GetCharacters/GetCharactersMsgReq.h"
#include "Messages/GetCharacters/GetCharactersMsgResp.h"
#include "Messages/Other/OtherMsg.h"
#include "Messages/RemoveCharacter/DisconnectCharacterMsg.h"
#include "Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "Messages/SelectCharacter/SelectCharacterMsgResp.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "Messages/TransformMessages/TransformMsgResp.h"
#include "Time/TimeMeasurer.h"

#define Case(x, y)                              \
    case x:                                     \
        result = GetIMessage<y>(socket, error); \
        break

namespace Network
{
Receiver::Receiver(Utils::Time::CTimeMeasurer& tm, ISDLNetWrapperPtr sdlNetWrapper)
    : sdlNetWrapper_(sdlNetWrapper)
    , timer_(tm)
    , recvBytes_(0)
{
    timer_.AddOnTickCallback(std::bind(&Receiver::PrintRecvBytesPerSec, this));
}

std::shared_ptr<IMessage> Receiver::Receive(TCPsocket socket, RecvError& error)
{
    DEBUG_LOG("Times test : " + std::to_string(clock() * 1000.0f / (float)CLOCKS_PER_SEC));

    error = RecvError::None;

    if (sdlNetWrapper_->SocketReady((SDLNet_GenericSocket)socket) == 0)
        return nullptr;

    // DEBUG_LOG("Receiver::Receive");
    std::shared_ptr<IMessage> result;

    // char buffer[512];
    MessageHeader header;
    int recvBytes = sdlNetWrapper_->RecvTcp(socket, &header, sizeof(header));
    DEBUG_LOG(Network::to_string(header.msgType));
    // DEBUG_LOG(std::string("Receive header, msg type : ") + std::to_string(header.msgType));

    if (recvBytes <= 0)
    {
        DEBUG_LOG("Recv header bytes : -1, Disconnect.");
        error = RecvError::Disconnect;
        return nullptr;
    }

    recvBytes_ += recvBytes;

    switch (header.msgType)
    {
        Case(MessageTypes::ConnectionMsg, ConnectionMessage);
        Case(MessageTypes::DisconnectCharacter, DisconnectCharacterMsg);
        Case(MessageTypes::TransformReq, TransformMsgReq);
        Case(MessageTypes::TransformResp, TransformMsgResp);
        Case(MessageTypes::Authentication, AuthenticationMessage);
        Case(MessageTypes::SelectCharacterReq, SelectCharacterMsgReq);
        Case(MessageTypes::SelectCharacterResp, SelectCharacterMsgResp);
        Case(MessageTypes::GetCharactersReq, GetCharactersMsgReq);
        Case(MessageTypes::GetCharactersResp, GetCharactersMsgResp);
        Case(MessageTypes::GetCharacterDataReq, GetCharacterDataMsgReq);
        Case(MessageTypes::GetCharacterDataResp, GetCharacterDataMsgResp);
        Case(MessageTypes::GetCharactersDataReq, GetCharactersDataMsgReq);
        Case(MessageTypes::Other, OtherMsg);
    }
    return result;
}

void Receiver::PrintRecvBytesPerSec()
{
    auto recvPerSec = recvBytes_;  // / 1000;
    DEBUG_LOG("Recv : " + std::to_string(recvPerSec) + " B/s");
    std::cout << " Recv : " << std::to_string(recvPerSec) << " B / s" << std::endl;
    recvBytes_ = 0;
}
}  // namespace Network