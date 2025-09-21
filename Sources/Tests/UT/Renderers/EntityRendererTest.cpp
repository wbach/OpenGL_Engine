
#include <GameEngine/Resources/Models/Model.h>
#include <Utils/MeasurementHandler.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <memory>
#include <mutex>
#include <optional>

#include "Camera/Frustrum.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Renderers/Objects/Entity/ConcreteEntityRenderer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "Objects/GameObject.h"
#include "Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "Tests/UT/EngineBasedTest.h"
#include "Types.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace GraphicsApi;

namespace GameEngine
{
namespace UT
{
const mat4 INDENITY_MATRIX(1.f);
const vec3 MATERIAL_AMBIENT{0.f, 0.f, 1.f};
const vec3 MATERIAL_DIFFUSE{1.f, 0.f, 1.f};
const vec3 MATERIAL_SPECULAR{1.f, 1.f, 1.f};

struct EntityRendererShould : public EngineBasedTest
{
    void SetUp() override
    {
        EngineBasedTest::SetUp();

        EXPECT_CALL(*rendererFactory, create(_)).WillOnce([&](RendererContext& ctx) {
            rendererContext             = &ctx;
            auto concreteEntityRenderer = std::make_unique<ConcreteEntityRenderer>(ctx);
            return concreteEntityRenderer;
        });

        engineContext->GetRenderersManager().Init();
        engineContext->GetThreadSync().Stop();

        ON_CALL(*graphicsApi, PrepareMatrixToLoad(_)).WillByDefault(Return(INDENITY_MATRIX));
        ON_CALL(*modelLoaderMock, CheckExtension(_)).WillByDefault(Return(true));
        ON_CALL(*modelLoaderMock, ParseFile(_)).WillByDefault(Return(true));
        ON_CALL(*modelLoaderMock, Create()).WillByDefault([&]() { return CreateModel(); });
    }

    void ExpectRender()
    {
        EXPECT_CALL(*graphicsApi, BindShaderBuffer(_)).Times(AtLeast(1));
        EXPECT_CALL(*graphicsApi, RenderMesh(_)).Times(AtLeast(1));
    }

    GameObject* AddGameObject(const std::string& name = "TestGameObject")
    {
        LOG_DEBUG << "AddGameObject start";

        auto entity = scene->CreateGameObject(name);
        entity->AddComponent<Components::RendererComponent>().AddModel("Meshes/sphere.obj");
        auto entityPtr = entity.get();
        scene->AddGameObject(std::move(entity));

        LOG_DEBUG << "AddGameObject done";

        return entityPtr;
    }

    std::unique_ptr<Model> CreateModel()
    {
        LOG_DEBUG << "CreateModel start";
        auto model = std::make_unique<Model>();
        Mesh mesh(GraphicsApi::RenderType::TRIANGLES, *graphicsApi);

        Material m;
        m.ambient  = MATERIAL_AMBIENT;
        m.diffuse  = MATERIAL_DIFFUSE;
        m.specular = MATERIAL_SPECULAR;
        mesh.SetMaterial(m);
        model->AddMesh(std::move(mesh));
        LOG_DEBUG << "CreateModel end";
        return model;
    }

    void LoadAllGpuTask()
    {
        LOG_DEBUG << "LoadAllGpuTask";
        auto& gpuLoader = engineContext->GetGpuResourceLoader();
        while (gpuLoader.CountObjectsToAdd() > 0 or gpuLoader.CountObjectsToUpdate() > 0 or gpuLoader.CountObjectsToRelease() > 0)
        {
            LOG_DEBUG << "RuntimeGpuTasks";
            engineContext->GetGpuResourceLoader().RuntimeGpuTasks();
        }
        LOG_DEBUG << "LoadAllGpuTask end";
    }

    Time time_;
    Frustrum frustrum;
    Projection projection_;
    GpuResourceLoaderMock gpuResourceLoaderMock;

    //  std::unique_ptr<Model> model_;
    mat4 transformToShader_;

    RendererContext* rendererContext;
};

TEST_F(EntityRendererShould, RenderSingleObject)
{
    auto go = AddGameObject();
    go->SetWorldPosition(vec3(10));
    scene->FullUpdate(0.1f);  // Apply object addition etc
    EXPECT_CALL(*graphicsApi, CreateMesh(_, _)).WillRepeatedly(Return(GraphicsApi::ID(IdPool.getId())));
    LoadAllGpuTask();

    ExpectRender();

    LOG_DEBUG << "Render";
    engineContext->GetRenderersManager().renderScene(*scene);
    LOG_DEBUG << "Done";

    SUCCEED();
}

TEST_F(EntityRendererShould, ParallelAddRemove)
{
    auto oldVerbose                = ::testing::GMOCK_FLAG(verbose);
    ::testing::GMOCK_FLAG(verbose) = "error";

    AddGameObject();
    ON_CALL(*graphicsApi, CreateMesh(_, _)).WillByDefault(Return(GraphicsApi::ID(IdPool.getId())));
    LoadAllGpuTask();

    std::atomic<bool> addStop{false};
    std::atomic<bool> removeStop{false};

    std::mutex objectMutex;
    using CreatedObjectContainer = std::vector<std::pair<GameObject*, IdType>>;
    CreatedObjectContainer objects;

  auto popRandomItem = [](CreatedObjectContainer& vec) -> std::optional<std::pair<GameObject*, IdType>> {
        if (vec.empty()) return std::nullopt;

        float r = std::clamp(getRandomFloat(), 0.f, std::nextafter(1.f, 0.f));
        size_t index = static_cast<size_t>(r * vec.size());

        auto item = vec[index];
        vec.erase(vec.begin() + index);
        return item;
    };

    std::thread gpuThread([&]() {
        while (!addStop or !removeStop)
        {
            LoadAllGpuTask();
            engineContext->GetRenderersManager().renderScene(*scene);

            LOG_DEBUG << "EnityRendererdMeshes " << rendererContext->measurmentHandler_.GetValue("EnityRendererdMeshes");
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
    });

    std::thread addThread([&]() {
        for (int i = 0; i < 1000000; ++i)
        {
            {
                std::lock_guard<std::mutex> lk(objectMutex);
                auto go = AddGameObject();
                objects.push_back({go, go->GetId()});
                LOG_DEBUG << "Add go: " << go->GetName() << " id=" << go->GetId();
            }

            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 1000)));
        }
        addStop = true;
    });

    std::thread logicThread([&]() {
        while (!addStop or !objects.empty())
        {
            std::optional<std::pair<GameObject*, IdType>> maybeGo;
            {
                std::lock_guard<std::mutex> lk(objectMutex);
                maybeGo = popRandomItem(objects);
            }

            if (maybeGo)
            {
                LOG_DEBUG << "Remove go: id=" << maybeGo->second;
                LOG_DEBUG << "Ptr is set id=" << maybeGo->second;
                LOG_DEBUG << "Check id=" << maybeGo->first->GetId();

                if (maybeGo->first)
                {
                    scene->RemoveGameObject(*maybeGo->first);
                }
            }
            scene->FullUpdate(0.1f);
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 1000)));
        }
        removeStop = true;
    });

    addThread.join();
    logicThread.join();
    gpuThread.join();

    // Nie sprawdzamy ASSERT_EQ – crash = fail
    LOG_DEBUG << "TestDone";
    SUCCEED();

    ::testing::GMOCK_FLAG(verbose) = oldVerbose;
}
}  // namespace UT
}  // namespace GameEngine
