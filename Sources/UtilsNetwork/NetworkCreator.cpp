#include "Logger/Log.h"
#include "NetworkUtils.h"
#include "ServerCreator.h"

namespace Network
{
NetworkCreator::NetworkCreator(ISDLNetWrapper& sdlNetWrapper)
    : sdlNetWrapper_(sdlNetWrapper)
    , isCreated(false)
{
}

NetworkCreator::~NetworkCreator()
{
    if (!isCreated)
        return;

    SDLNet_TCP_Close(context_.socket);
    SDLNet_FreeSocketSet(context_.socketSet);
    SDLNet_Quit();
}

bool NetworkCreator::Init()
{
    if (sdlNetWrapper_.Init() != -1)
        return true;

    ERROR_LOG("Failed to intialise SDL_net: " + std::string(sdlNetWrapper_.GetError()));
    return false;
}

bool NetworkCreator::AllocSocketSet(int count)
{
    context_.socketSet = sdlNetWrapper_.AllocSocketSet(count);

    if (context_.socketSet == nullptr)
    {
        ERROR_LOG("Failed to allocate the socket set:: " + std::string(sdlNetWrapper_.GetError()));
        return false;
    }

    std::string str = "Allocated socket set with size:  " + std::to_string(context_.maxClients + 1) + ", of which " + std::to_string(context_.maxClients) + " are availble for use by clients.";
    DEBUG_LOG(str);
    return true;
}

bool NetworkCreator::ResolveHost(const char* hostName)
{
    int hostResolved = sdlNetWrapper_.ResolveHost(&context_.serverIP, hostName, context_.port);

    if (hostResolved == -1)
    {
        ERROR_LOG("Failed to resolve the server host: " + std::string(sdlNetWrapper_.GetError()));
        return false;
    }

    DEBUG_LOG("Successfully resolved server host to IP: " + UtilsNetwork::IpAddressToString(context_.serverIP));
    return true;
}

bool NetworkCreator::ResolveIp()
{
    const char* ipResolved = sdlNetWrapper_.ResolveIP(&context_.serverIP);

    if (ipResolved == nullptr)
    {
        ERROR_LOG("Failed to resolve ip: " + std::string(sdlNetWrapper_.GetError()));
        return false;
    }

    DEBUG_LOG("Ip resolved : " + std::string(ipResolved));
    return true;
}

bool NetworkCreator::OpenTcp()
{
    // Try to open the server socket
    context_.socket = sdlNetWrapper_.TCPOpen(&context_.serverIP);

    if (!context_.socket)
    {
        ERROR_LOG("Failed to open the server socket: " + std::string(sdlNetWrapper_.GetError()));
        return false;
    }

    DEBUG_LOG("Sucessfully created server socket.");
    return true;
}

bool NetworkCreator::AddSocketTcp()
{
    int i = sdlNetWrapper_.TCPAddSocket(context_.socketSet, context_.socket);
    return i != -1;
}

}  // namespace Network
