
#include <Utils/MeasurementHandler.h>
#include <gtest/gtest.h>

#include <functional>

#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Renderers/Objects/Entity/EntityRenderer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"
#include "GameEngineTests/Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "GameEngineTests/Tests/Mocks/Resources/ResourcesManagerMock.h"
#include "GameEngineTests/Tests/UT/Components/BaseComponent.h"

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

struct EntityRendererShould : public BaseComponentTestSchould
{
    EntityRendererShould()
        : graphicsMock_(new GraphicsApiMock())
        , engineContext_(std::unique_ptr<IGraphicsApi>(graphicsMock_), std::make_unique<PhysicsApiMock>())
        , scene_("testScene")
        , mesh_(GraphicsApi::RenderType::TRIANGLES, engineContext_.GetGraphicsApi())
        , context_(projection_, frustrum_, engineContext_.GetGraphicsApi(), gpuResourceLoaderMock_, measurmentHandler_, time_)
    {
    }
    void SetUp()
    {
        EXPECT_CALL(resourceManagerMock_, GetGraphicsApi()).WillRepeatedly(ReturnRef(*graphicsMock_));
        EXPECT_CALL(*graphicsMock_, GetWindowApi()).WillRepeatedly(ReturnRef(windowApiMock_));

        context_.projection_.CreateProjectionMatrix();

        scene_.InitResources(engineContext_);
        scene_.Init();
        sut_.reset(new EntityRenderer(context_));
    }
    void ExpectCalls()
    {
    }
    void ExpectShaderInit()
    {
    }

    void ExpectRender()
    {
        EXPECT_CALL(*graphicsMock_, BindShaderBuffer(_)).Times(1);
        EXPECT_CALL(*graphicsMock_, RenderMesh(_)).Times(1);
    }

    void AddGameObject()
    {
        EXPECT_CALL(*graphicsMock_, PrepareMatrixToLoad(_)).WillRepeatedly(Return(INDENITY_MATRIX));
        EXPECT_CALL(resourceManagerMock_, GetGpuResourceLoader()).WillRepeatedly(ReturnRef(gpuResourceLoaderMock_));
        CreateModel();
        EXPECT_CALL(gpuResourceLoaderMock_, AddObjectToGpuLoadingPass(_)).Times(2);
        EXPECT_CALL(resourceManagerMock_, LoadModel(_, _)).WillOnce(Return(&model_));

        auto entity = scene_.CreateGameObject();
        entity->AddComponent<Components::RendererComponent>().AddModel("Meshes/sphere.obj");
        sut_->subscribe(*entity);
        transformToShader_ = entity->GetWorldTransform().GetMatrix() * mesh_.GetMeshTransform();
        scene_.AddGameObject(std::move(entity));
    }

    void CreateModel()
    {
        Material m;
        m.ambient  = MATERIAL_AMBIENT;
        m.diffuse  = MATERIAL_DIFFUSE;
        m.specular = MATERIAL_SPECULAR;
        mesh_.SetMaterial(m);
        model_.AddMesh(mesh_);
    }
    Time time_;
    GraphicsApi::GraphicsApiMock* graphicsMock_;
    EngineContext engineContext_;
    GpuResourceLoaderMock gpuResourceLoaderMock_;
    ResourceManagerMock resourceManagerMock_;
    Projection projection_;
    GameEngine::Scene scene_;
    Model model_;
    Mesh mesh_;
    mat4 transformToShader_;
    RendererContext context_;
    Utils::MeasurementHandler measurmentHandler_;
    std::unique_ptr<EntityRenderer> sut_;
};

// TEST_F(EntityRendererShould, RenderWithoutObjectsTest)
//{
//    ExpectShaderInit();
//    sut_->Init();
//    AddGameObject();
//    ExpectRender();
//    sut_->Render(scene_, time_);
//}

}  // namespace UT
}  // namespace GameEngine
