#pragma once
#include <GameEngine/Scene/ISceneFactory.h>
#include <gmock/gmock.h>

class SceneFactoryMock : public GameEngine::ISceneFactory
{
public:
    MOCK_METHOD(GameEngine::ScenePtr, Create, (const std::string&), (override));
    MOCK_METHOD(GameEngine::ScenePtr, Create, (uint32), (override));
    MOCK_METHOD(GameEngine::ScenePtr, CreateSceneBasedOnFile, (const GameEngine::File&), (override));

    MOCK_METHOD(uint32, ScenesSize, (), (override));
    MOCK_METHOD(const std::string&, GetSceneName, (uint32), (override));
    MOCK_METHOD(uint32, GetSceneId, (const std::string&), (override));
    MOCK_METHOD(const GameEngine::IdMap&, GetAvaiableScenes, (), (const, override));

    MOCK_METHOD(bool, IsExist, (uint32), (const, override));
    MOCK_METHOD(bool, IsExist, (const std::string&), (const, override));

    MOCK_METHOD(void, SetEngineContext, (GameEngine::EngineContext&), (override));
    MOCK_METHOD(void, Clear, (), (override));
};
