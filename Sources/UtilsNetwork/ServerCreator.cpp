#include "ServerCreator.h"
#include "Logger/Log.h"
#include "NetworkUtils.h"

namespace Network
{
ServerCreator::ServerCreator(ISDLNetWrapper& sdlNetWrapper)
    : NetworkCreator(sdlNetWrapper)
{
}

std::optional<ConectContext> ServerCreator::Create(uint32 maxClients, uint32 port)
{
    context_.maxClients = maxClients;
    context_.port       = port;

    if (not Init())
        return {};
    if (not AllocSocketSet(context_.maxClients + 1))
        return {};
    if (not ResolveHost())
        return {};
    if (not OpenTcp())
        return {};
    if (not AddSocketTcp())
        return {};

    /* LOG TO FIX*/  LOG_ERROR << ("Connection is open.");
    isCreated = true;

    return context_;
}

}  // namespace Network
