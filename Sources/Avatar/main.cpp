#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include "OpenGLApi/OpenGLApi.h"
#include "StartGame.h"
#ifndef USE_GNU
#include "DirectXApi/DirectXApi.h"
#endif

#include <GLM/GLMUtils.h>

const std::string configFile = "./Conf.xml";

int main(int, char**)
{
    CLogger::Instance().EnableLogs(LogginLvl::ErrorWarningInfoDebug);
    CLogger::Instance().ImmeditalyLog();

    GameEngine::ReadFromFile(configFile);
    std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi;

    Rotation r(DegreesVec3(256, 354, 0));

    ////  auto cameraRotation = r.value_;

    float pitch = 20.f; float yaw = 210.f;
    vec4 point(1.f, 2.f, 3.f, 0.f);

    Rotation rr(DegreesVec3(pitch, yaw, 0));
    auto rotation = glm::rotate(-glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
    rotation *= glm::rotate(-glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));

    auto rotation2 = glm::rotate(glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
    rotation2 *= glm::rotate(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));

    DEBUG_LOG("R1 " + std::to_string(rotation));
    DEBUG_LOG("R2 " + std::to_string(rotation2));
    DEBUG_LOG("R3 " + std::to_string(glm::inverse(rotation2)));
    DEBUG_LOG("R4 " + std::to_string(glm::mat4_cast(rr.value_)));

    DEBUG_LOG("P1 " + std::to_string(rotation * point));
    DEBUG_LOG("P2 " + std::to_string(rotation2 * point));
    DEBUG_LOG("P3 " + std::to_string(glm::inverse(rotation2) * point));
    DEBUG_LOG("P4 " + std::to_string(rr.value_ * point));
    DEBUG_LOG("P5 " + std::to_string(glm::mat4_cast(rr.value_) * point));

    return 0;

    //  //auto rotation = glm::rotate(-r.GetEulerRadians().value.y, glm::vec3(0.f, 1.f, 0.f));
    //  //rotation *= glm::rotate(-r.GetEulerRadians().value.x, glm::vec3(1.f, 0.f, 0.f));
    //  //cameraRotation = glm::quat_cast(rotation);
    //  vec3 cameraForwardVector = glm::normalize(cameraRotation * VECTOR_FORWARD);
    //  vec3 cameraUpVector = glm::normalize(cameraRotation * VECTOR_UP);

    //  Utils::PrintMatrix("UpdateLightViewMatrix ", Utils::CreateLightViewMatrix(glm::vec3(1, 1, 1), glm::vec3(192,
    //  168, 1))); Utils::PrintMatrix("UpdateLightViewMatrix ", Utils::CreateLightViewMatrix(glm::vec3(1, 1, -1),
    //  glm::vec3(192, 168, 1))); Utils::PrintVector("GetForwardVec ", cameraForwardVector);
    //  Utils::PrintVector("GetForwardVec ", cameraUpVector);
    //  system("pause");
    //  return 0;

#ifndef USE_GNU
    if (EngineConf.renderer.graphicsApi == "OpenGL")
    {
        graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
    }
    else if (EngineConf.renderer.graphicsApi == "DirectX11")
    {
        graphicsApi = std::make_unique<DirectX::DirectXApi>();
    }
    else
    {
        graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
    }
#else
    if (EngineConf.renderer.graphicsApi != "OpenGL")
    {
        DEBUG_LOG("GNU support only OpenGL");
    }
    graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
#endif
    graphicsApi->SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));
    AvatarGame::Start(std::move(graphicsApi));
    return 0;
}
