#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include "StartGame.h"


#include <GLM/GLMUtils.h>

const std::string configFile = "./Conf.xml";

int main(int, char**)
{
    AvatarGame::Start();
    return 0;
}
