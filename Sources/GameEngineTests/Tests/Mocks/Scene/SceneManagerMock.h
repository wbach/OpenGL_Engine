#pragma once
#include <GameEngine/Scene/ISceneManager.h>
#include <gmock/gmock.h>

class SceneManagerMock : public GameEngine::ISceneManager
{
public:
    MOCK_METHOD(GameEngine::Scene*, GetActiveScene, (), (override));
    MOCK_METHOD(void, SetOnSceneLoadDone, (OnSceneLoadDoneCallback), (override));
    MOCK_METHOD(void, Update, (), (override));
    MOCK_METHOD(void, SetActiveScene, (const std::string&), (override));
    MOCK_METHOD(void, Reset, (), (override));
    MOCK_METHOD(void, StopThread, (), (override));
    MOCK_METHOD(const GameEngine::IdMap&, GetAvaiableScenes, (), (const, override));
};
