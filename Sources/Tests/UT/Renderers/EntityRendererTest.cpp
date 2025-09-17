
#include <Utils/MeasurementHandler.h>
#include <gtest/gtest.h>

#include <memory>

#include "Camera/Frustrum.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Renderers/Objects/Entity/ConcreteEntityRenderer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "Tests/UT/EngineBasedTest.h"
#include "gmock/gmock.h"
#include <GameEngine/Resources/Models/Model.h>

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

        rendererContext = std::make_unique<RendererContext>(projection_, frustrum, *graphicsApi,
                                                            engineContext->GetGpuResourceLoader(), measurmentHandler_, time_);
        rendererContext->projection_.CreateProjectionMatrix();

        EXPECT_CALL(*rendererFactory, create(_))
            .WillOnce(
                [&](RendererContext& ctx)
                {
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

    void AddGameObject()
    {
        assert(model_ != nullptr);

        EXPECT_CALL(*graphicsApi, PrepareMatrixToLoad(_)).WillRepeatedly(Return(INDENITY_MATRIX));
        EXPECT_CALL(*resourceManager, GetGpuResourceLoader()).WillRepeatedly(ReturnRef(gpuResourceLoaderMock));
        CreateModel();

        EXPECT_CALL(*resourceManager, LoadModel(_, _))
            .WillOnce(
                [&](const File&, const LoadingParameters&)
                {
                    // During this step object is added to gpu pass
                    engineContext->GetGpuResourceLoader().AddObjectToGpuLoadingPass(*model_);
                    return model_.get();
                });


        auto entity = scene->CreateGameObject();
        entity->AddComponent<Components::RendererComponent>().AddModel("Meshes/sphere.obj");
        transformToShader_ = entity->GetWorldTransform().GetMatrix() * model_->GetMeshes().front().GetMeshTransform();
        scene->AddGameObject(std::move(entity));

        LOG_DEBUG << "AddGameObject done";
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
    Utils::MeasurementHandler measurmentHandler_;

    std::unique_ptr<RendererContext> rendererContext;
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

}  // namespace UT
}  // namespace GameEngine
