#include "GameServer/GameServer.h"

int main(int argc, char** argv)
{
    // CLogger::Instance().EnableLogs();

    DEBUG_LOG("RPG GameServer v 0.1");

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindow("My Game Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    GameServer::GameServer();
    SDL_Quit();
    return 0;
}
