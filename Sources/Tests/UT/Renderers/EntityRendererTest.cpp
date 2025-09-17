
#include <GameEngine/Resources/Models/Model.h>
#include <Utils/MeasurementHandler.h>
#include <gtest/gtest.h>

#include <memory>
#include <mutex>

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
    void SetUp()
    {
        EngineBasedTest::SetUp();

        EXPECT_CALL(*rendererFactory, create(_))
            .WillOnce(
                [&](RendererContext& ctx)
                {
                    rendererContext             = &ctx;
                    auto concreteEntityRenderer = std::make_unique<ConcreteEntityRenderer>(ctx);
                    return concreteEntityRenderer;
                });

        engineContext->GetRenderersManager().Init();

        model_ = CreateModel();
    }

    void ExpectRender()
    {
        EXPECT_CALL(*graphicsApi, BindShaderBuffer(_)).Times(AtLeast(1));
        EXPECT_CALL(*graphicsApi, RenderMesh(_)).Times(AtLeast(1));
    }

    GameObject* AddGameObject()
    {
        assert(model_ != nullptr);

        EXPECT_CALL(*graphicsApi, PrepareMatrixToLoad(_)).WillRepeatedly(Return(INDENITY_MATRIX));
        EXPECT_CALL(*resourceManager, GetGpuResourceLoader()).WillRepeatedly(ReturnRef(gpuResourceLoaderMock));
        CreateModel();

        EXPECT_CALL(*resourceManager, LoadModel(File("Meshes/sphere.obj"), _))
            .WillRepeatedly(
                [&](const File&, const LoadingParameters&)
                {
                    // During this step object is added to gpu pass
                    if (not model_->GetGraphicsObjectId())
                        engineContext->GetGpuResourceLoader().AddObjectToGpuLoadingPass(*model_);
                    return model_.get();
                });

        auto entity = scene->CreateGameObject();
        entity->AddComponent<Components::RendererComponent>().AddModel("Meshes/sphere.obj");
        transformToShader_ = entity->GetWorldTransform().GetMatrix() * model_->GetMeshes().front().GetMeshTransform();
        auto entityPtr     = entity.get();
        scene->AddGameObject(std::move(entity));

        LOG_DEBUG << "AddGameObject done";

        return entityPtr;
    }

    std::unique_ptr<Model> CreateModel()
    {
        auto model = std::make_unique<Model>();
        Mesh mesh(GraphicsApi::RenderType::TRIANGLES, *graphicsApi);

        Material m;
        m.ambient  = MATERIAL_AMBIENT;
        m.diffuse  = MATERIAL_DIFFUSE;
        m.specular = MATERIAL_SPECULAR;
        mesh.SetMaterial(m);
        model->AddMesh(std::move(mesh));
        return model;
    }

    void LoadAllGpuTask()
    {
        auto& gpuLoader = engineContext->GetGpuResourceLoader();
        while (gpuLoader.CountObjectsToAdd() > 0 or gpuLoader.CountObjectsToUpdate() > 0 or gpuLoader.CountObjectsToRelease() > 0)
        {
            engineContext->GetGpuResourceLoader().RuntimeGpuTasks();
        }
    }

    Time time_;
    Frustrum frustrum;
    Projection projection_;
    GpuResourceLoaderMock gpuResourceLoaderMock;

    std::unique_ptr<Model> model_;
    mat4 transformToShader_;

    RendererContext* rendererContext;
};

TEST_F(EntityRendererShould, RenderSingleObject)
{
    AddGameObject();
    EXPECT_CALL(*graphicsApi, CreateMesh(_, _)).WillRepeatedly(Return(GraphicsApi::ID(IdPool.getId())));
    LoadAllGpuTask();

    ExpectRender();

    LOG_DEBUG << "Render";
    engineContext->GetRenderersManager().renderScene(*scene);
    LOG_DEBUG << "Done";

    SUCCEED();
}

TEST_F(EntityRendererShould, DISABLED_ParallelAddRemove)
{
    AddGameObject();
    EXPECT_CALL(*graphicsApi, CreateMesh(_, _)).WillRepeatedly(Return(GraphicsApi::ID(IdPool.getId())));
    LoadAllGpuTask();

    std::atomic<bool> addStop{false};
    std::atomic<bool> removeStop{false};

    std::mutex objectMutex;
    std::vector<GameObject*> objects;
    auto popRandomItem = [](std::vector<GameObject*>& vec) -> GameObject*
    {
        if (vec.empty())
            return nullptr;

        size_t index = static_cast<size_t>(getRandomFloat() * vec.size());
        if (index >= vec.size())
            index = vec.size() - 1;

        GameObject* item = vec[index];
        vec.erase(vec.begin() + index);
        return item;
    };

    std::thread gpuThread(
        [&]()
        {
            while (!addStop or !removeStop)
            {
                EXPECT_CALL(*graphicsApi, CreateMesh(_, _)).WillRepeatedly(Return(GraphicsApi::ID(IdPool.getId())));
                LoadAllGpuTask();
                engineContext->GetRenderersManager().renderScene(*scene);

                LOG_DEBUG << "EnityRendererdMeshes " << rendererContext->measurmentHandler_.GetValue("EnityRendererdMeshes");
                std::this_thread::sleep_for(std::chrono::microseconds(50));
            }
        });

    std::thread addThread(
        [&]()
        {
            for (int i = 0; i < 100; ++i)
            {
                {
                    std::lock_guard<std::mutex> lk(objectMutex);
                    objects.push_back(AddGameObject());
                }

                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100)));
            }
            addStop = true;
        });

    std::thread logicThread(
        [&]()
        {
            while (!addStop or !objects.empty())
            {
                GameObject* go = nullptr;
                {
                    std::lock_guard<std::mutex> lk(objectMutex);
                    go = popRandomItem(objects);
                }
                if (go)
                {
                    scene->RemoveGameObject(*go);
                }

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
}
}  // namespace UT
}  // namespace GameEngine
