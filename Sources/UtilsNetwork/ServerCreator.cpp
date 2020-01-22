#include "ServerCreator.h"
#include "Logger/Log.h"
#include "NetworkUtils.h"

namespace Network
{
ServerCreator::ServerCreator(ISDLNetWrapper& sdlNetWrapper)
    : NetworkCreator(sdlNetWrapper)
{
}

ConectContext ServerCreator::Create(uint32 maxClients, uint32 port)
{
    context_.maxClients = maxClients;
    context_.port       = port;

    if (not Init())
        return context_;
    if (not AllocSocketSet(context_.maxClients + 1))
        return context_;
    if (not ResolveHost())
        return context_;
    if (not OpenTcp())
        return context_;
    if (not AddSocketTcp())
        return context_;

    DEBUG_LOG("Connection is open.");
    isCreated = true;

    return context_;
}

}  // namespace Network
