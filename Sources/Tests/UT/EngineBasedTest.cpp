#include "EngineBasedTest.h"

#include <future>
#include <memory>

#include "GameEngine/Engine/EngineContext.h"
#include "Logger/Log.h"
#include "GameEngine/Resources/Models/WBLoader/IModelLoaderFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Tests/Mocks/Physics/PhysicsApiMock.h"
#include "Tests/Mocks/Resources/ModelLoaderFactoryMock.h"
#include "Tests/Mocks/Resources/ResourceManagerFactoryMock.h"
#include "Tests/Mocks/Resources/ResourcesManagerMock.h"
#include "Tests/Mocks/Resources/TextureLoaderMock.h"
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
    auto threadSyncMock             = std::make_unique<Utils::Thread::MockThreadSync>();

    threadSync                         = threadSyncMock.get();
    graphicsApi                        = graphicsApiMock.get();
    rendererFactory                    = rendererFactoryMock.get();
    physicsApi                         = physicsApiMock.get();
    auto sceneFactoryMockPtr           = sceneFactoryMock.get();
    auto resourceManagerFactoryMockPtr = resourceManagerFactoryMock.get();

    EXPECT_CALL(*graphicsApi, GetWindowApi()).WillRepeatedly(ReturnRef(windowApiMock));
    EXPECT_CALL(windowApiMock, GetDisplayModes()).WillRepeatedly(ReturnRef(displayModes));
    EXPECT_CALL(windowApiMock, GetInputManager()).WillRepeatedly(ReturnRef(inputManagerMock));
    EXPECT_CALL(*graphicsApi, CreateShaderBuffer(_, _)).WillRepeatedly(Return(GraphicsApi::ID(IdPool.getId())));
    EXPECT_CALL(*graphicsApi, CreateShader(_)).WillRepeatedly(Return(GraphicsApi::ID(IdPool.getId())));

    LOG_DEBUG << "EngineBasedTest::CreateEngineContext";
    engineContext = std::make_unique<EngineContext>(std::move(physicsApiMock), std::move(sceneFactoryMock),
                                                    std::move(resourceManagerFactoryMock), std::move(rendererFactoryMock),
                                                    std::move(threadSyncMock), std::move(graphicsApiMock));

    LOG_DEBUG << "EngineBasedTest::CreateScene";

    // make real scene maganger to manage model and instances
    auto modelLoaderFactoryMock = std::make_unique<ModelLoaderFactoryMock>();
    auto textureLoaderMock      = std::make_unique<TextureLoaderMock>();
    textureLoader               = textureLoaderMock.get();

    EXPECT_CALL(*modelLoaderFactoryMock, createLoaders())
        .WillOnce(::testing::Invoke(
            [&]()
            {
                auto loader     = std::make_unique<LoaderMock>(*graphicsApi, *textureLoader);
                modelLoaderMock = loader.get();
                LoadersVector v;
                v.push_back(std::move(loader));
                return v;
            }));
    auto resourceManager = std::make_unique<GameEngine::ResourceManager>(
        *graphicsApi, engineContext->GetGpuResourceLoader(), std::move(textureLoaderMock), std::move(modelLoaderFactoryMock));

    auto scenePtr = std::make_unique<Scene>("TestScene");
    // Scene resourceManager
    EXPECT_CALL(*resourceManagerFactoryMockPtr, create()).WillOnce(Return(ByMove(std::move(resourceManager))));
    scenePtr->InitResources(*engineContext);

    LOG_DEBUG << "EngineBasedTest::LoadScene";
    auto sceneLoaderResouceManager    = std::make_unique<ResourceManagerMock>();
    auto sceneLoaderResouceManagerPtr = sceneLoaderResouceManager.get();

    EXPECT_CALL(*sceneFactoryMockPtr, IsExist(::testing::A<uint32>())).WillRepeatedly(Return(true));
    EXPECT_CALL(*resourceManagerFactoryMockPtr, create()).WillOnce(Return(ByMove(std::move(sceneLoaderResouceManager))));
    EXPECT_CALL(*sceneLoaderResouceManagerPtr, GetTextureLoader()).WillRepeatedly(ReturnRef(*textureLoader));
    EXPECT_CALL(*sceneFactoryMockPtr, Create(::testing::A<uint32>())).WillRepeatedly(Return(ByMove(std::move(scenePtr))));

    std::promise<void> sceneReady;
    engineContext->GetSceneManager().SetOnSceneLoadDone([&]() { sceneReady.set_value(); });
    engineContext->GetSceneManager().SetActiveScene(0);

    auto future = sceneReady.get_future();
    while (future.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready)
    {
        engineContext->GetSceneManager().Update();
    }
    scene = engineContext->GetSceneManager().GetActiveScene();
    EXPECT_NE(scene, nullptr);

    engineContext->GetSceneManager().StopThread();
    LOG_DEBUG << "EngineBasedTest::SetUp done";
}

EngineBasedTest::~EngineBasedTest()
{
    LOG_DEBUG << "";
}

void EngineBasedTest::TearDown()
{
    engineContext.reset();
    scene = nullptr;
}
