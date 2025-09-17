#include "EngineBasedTest.h"

#include <memory>

#include "Engine/EngineContext.h"
#include "Logger/Log.h"
#include "Tests/Mocks/Resources/ResourceManagerFactoryMock.h"
#include "Tests/Mocks/Physics/PhysicsApiMock.h"
#include "Tests/Mocks/Scene/SceneFactoryMock.h"
#include "gmock/gmock.h"

using namespace testing;

EngineBasedTest::EngineBasedTest()
{
}

void EngineBasedTest::SetUp()
{
    displayModes = {{.w = 640, .h = 480, .refreshRate = 60, .displayIndex = 0}};
    LOG_DEBUG << "EngineBasedTest::Setup";
    auto graphicsApiMock            = std::make_unique<GraphicsApiMock>();
    auto physicsApiMock             = std::make_unique<PhysicsApiMock>();
    auto sceneFactoryMock           = std::make_unique<SceneFactoryMock>();
    auto resourceManagerFactoryMock = std::make_unique<ResourceManagerFactoryMock>();
    auto rendererFactoryMock        = std::make_unique<RendererFactoryMock>();

    graphicsApi                        = graphicsApiMock.get();
    rendererFactory                    = rendererFactoryMock.get();
    auto sceneFactoryMockPtr           = sceneFactoryMock.get();
    auto resourceManagerFactoryMockPtr = resourceManagerFactoryMock.get();

    EXPECT_CALL(*graphicsApi, GetWindowApi()).WillRepeatedly(ReturnRef(windowApiMock));
    EXPECT_CALL(windowApiMock, GetDisplayModes()).WillRepeatedly(ReturnRef(displayModes));
    EXPECT_CALL(windowApiMock, GetInputManager()).WillRepeatedly(ReturnRef(inputManagerMock));
    EXPECT_CALL(*graphicsApi, CreateShaderBuffer(_, _)).WillRepeatedly(Return(GraphicsApi::ID(IdPool.getId())));
    EXPECT_CALL(*graphicsApi, CreateShader(_)).WillRepeatedly(Return(GraphicsApi::ID(IdPool.getId())));

    LOG_DEBUG << "EngineBasedTest::CreateEngineContext";
    engineContext =
        std::make_unique<EngineContext>(std::move(graphicsApiMock), std::move(physicsApiMock), std::move(sceneFactoryMock),
                                        std::move(resourceManagerFactoryMock), std::move(rendererFactoryMock));

    LOG_DEBUG << "EngineBasedTest::CreateScene";
    auto resourceManagerMock = std::make_unique<ResourceManagerMock>();
    resourceManager          = resourceManagerMock.get();

    auto scenePtr = std::make_unique<Scene>("TestScene");
    // Scene resourceManager
    EXPECT_CALL(*resourceManagerFactoryMockPtr, create()).WillOnce(Return(ByMove(std::move(resourceManagerMock))));
    EXPECT_CALL(*resourceManager, GetTextureLoader()).WillRepeatedly(ReturnRef(textureLoaderMock));
    scenePtr->InitResources(*engineContext);

    LOG_DEBUG << "EngineBasedTest::LoadScene";
    auto sceneLoaderResouceManager    = std::make_unique<ResourceManagerMock>();
    auto sceneLoaderResouceManagerPtr = sceneLoaderResouceManager.get();

    EXPECT_CALL(*sceneFactoryMockPtr, IsExist(::testing::A<uint32>())).WillRepeatedly(Return(true));
    EXPECT_CALL(*resourceManagerFactoryMockPtr, create()).WillOnce(Return(ByMove(std::move(resourceManagerMock))));
    EXPECT_CALL(*sceneLoaderResouceManagerPtr, GetTextureLoader()).WillRepeatedly(ReturnRef(textureLoaderMock));
    EXPECT_CALL(*sceneFactoryMockPtr, Create(::testing::A<uint32>())).WillRepeatedly(Return(ByMove(std::move(scenePtr))));

    engineContext->GetSceneManager().SetActiveScene(0);
    engineContext->GetSceneManager().Update();
    scene = engineContext->GetSceneManager().GetActiveScene();
    EXPECT_NE(scene, nullptr);

    LOG_DEBUG << "EngineBasedTest::SetUp done";
}

EngineBasedTest::~EngineBasedTest()
{
}
