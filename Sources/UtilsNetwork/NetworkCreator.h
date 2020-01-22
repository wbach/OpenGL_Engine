#pragma once
#include "ConectContext.h"
#include "ISDLNetWrapper.h"
#include "SDLNetWrapper.h"

namespace Network
{
class NetworkCreator
{
public:
    NetworkCreator(ISDLNetWrapper& sdlNetWrapper);
    virtual ~NetworkCreator();

public:
    bool Init();
    bool AllocSocketSet(int count);
    bool ResolveHost(const char* hostName = nullptr);
    bool ResolveIp();
    bool OpenTcp();
    bool AddSocketTcp();

protected:
    ConectContext context_;
    ISDLNetWrapper& sdlNetWrapper_;
    bool isCreated;
};
}  // namespace Network
