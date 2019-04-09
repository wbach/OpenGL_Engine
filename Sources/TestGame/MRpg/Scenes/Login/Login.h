#pragma once
#include "../MRpgScene.h"
#include "Renderers/GUI/Text/GuiTextElement.h"
#include "Time/TimeMeasurer.h"

namespace Network
{
class CGateway;
}  // Network

namespace MmmoRpg
{
class LoginScene : public MRpgScene
{
public:
    LoginScene(Network::CGateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext);
    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float deltaTime) override;

private:
    void PrintLoginAndPassword();
    void ConnectToServer();
    std::string PasswordToStars();
    void SwapLoginPassword();
    void SwapCursor();

private:
    Utils::Time::CTimeMeasurer cursorTimer_;

    //GameEngine::GuiTextElement guiLogin_;
    //GameEngine::GuiTextElement guiPass_;

    std::string login_;
    std::string password_;

    bool loginOrPasswordInput;
    bool showCursor_;
};
}  // MmmoRpg
