#include <gtest/gtest.h>
#include <functional>
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Renderers/Objects/Entity/EntityRenderer.h"
#include "GameEngine/Renderers/Objects/Entity/EntityRendererDef.h"
#include "GameEngine/Renderers/Objects/Entity/Shaders/EntityShaderUniforms.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"
#include "GameEngineTests/Tests/Mocks/Renderers/FrameBuffer/FrameBufferMock.h"
#include "GameEngineTests/Tests/Mocks/Renderers/Objects/Shadows/ShadowFrameBufferMock.hpp"
#include "GameEngineTests/Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "GameEngineTests/Tests/Mocks/Resources/ResourcesManagerMock.h"
#include "GameEngineTests/Tests/Mocks/Shaders/ShaderFactoryMock.h"
#include "GameEngineTests/Tests/Mocks/Shaders/ShaderProgramMock.h"
#include "GameEngineTests/Tests/UT/Components/BaseComponent.h"

using namespace testing;
using namespace GraphicsApi;

namespace GameEngine
{
namespace UT
{
const uint32 SHADER_PROGRAM_ID = 12;
const mat4 VIEW_MATRIX;
const mat4 INDENITY_MATRIX;
const vec3 MATERIAL_AMBIENT{0.f, 0.f, 1.f};
const vec3 MATERIAL_DIFFUSE{1.f, 0.f, 1.f};
const vec3 MATERIAL_SPECULAR{1.f, 1.f, 1.f};

struct EntityRendererShould : public BaseComponentTestSchould
{
    EntityRendererShould()
        : graphicsMock_()
        , shaderFactoryMock_()
        , frameBufferMock_()
        , shadowFrameBufferMock_()
        , scene_("testScene")
        , mesh_(GraphicsApi::RenderType::TRIANGLES, graphicsMock_)
        , context_(projection_, graphicsMock_, frameBufferMock_, shadowFrameBufferMock_, shaderFactoryMock_,
                   std::bind(&EntityRendererShould::RenderFunction, this, std::placeholders::_1, std::placeholders::_2))
    {
    }
    void SetUp()
    {
        scene_.SetCamera(std::move(camera_));
        resourceManagerMock_ = new ResourceManagerMock();
        EXPECT_CALL(*resourceManagerMock_, GetGraphicsApi()).WillRepeatedly(ReturnRef(graphicsMock_));
        EXPECT_CALL(graphicsMock_, GetWindowApi()).WillRepeatedly(ReturnRef(windowApiMock_));

        scene_.CreateResourceManger(resourceManagerMock_);
        context_.projection_.CreateProjectionMatrix();

        EXPECT_CALL(shaderFactoryMock_, createImpl(Shaders::Entity)).WillOnce(Return(&shaderProgramMock_));

        scene_.SetRenderersManager(&renderersManager_);
        scene_.SetPhysicsApi(physicsApiMock_);
        scene_.Init();
        sut_.reset(new EntityRenderer(context_));
    }
    void RenderFunction(RendererFunctionType, RendererFunction)
    {
    }
    void ExpectCalls()
    {
    }
    void ExpectShaderInit()
    {
        EXPECT_CALL(shaderProgramMock_, Init());
    }

    void ExpectRender()
    {
        EXPECT_CALL(frameBufferMock_, BindToDraw()).Times(1);
        EXPECT_CALL(shaderProgramMock_, Start()).Times(1);
        EXPECT_CALL(graphicsMock_, BindShaderBuffer(_)).Times(1);
        EXPECT_CALL(graphicsMock_, RenderMesh(_)).Times(1);
        EXPECT_CALL(frameBufferMock_, UnBind()).Times(1);
    }

    void AddGameObject()
    {
        EXPECT_CALL(graphicsMock_, PrepareMatrixToLoad(_)).WillRepeatedly(Return(INDENITY_MATRIX));
        EXPECT_CALL(*resourceManagerMock_, GetGpuResourceLoader()).WillRepeatedly(ReturnRef(gpuResourceLoaderMock_));
        CreateModel();
        EXPECT_CALL(gpuResourceLoaderMock_, AddObjectToGpuLoadingPass(_)).Times(2);
        EXPECT_CALL(*resourceManagerMock_, LoadModel(_)).WillOnce(Return(&model_));

        auto entity = scene_.CreateGameObject();
        entity->AddComponent<Components::RendererComponent>().AddModel("Meshes/sphere.obj");
        sut_->Subscribe(entity.get());
        transformToShader_ = entity->worldTransform.GetMatrix() * mesh_.GetMeshTransform();
        scene_.AddGameObject(entity);
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
    GpuResourceLoaderMock gpuResourceLoaderMock_;
    GraphicsApi::GraphicsApiMock graphicsMock_;
    ShaderProgramMock shaderProgramMock_;
    ShaderFactoryMock shaderFactoryMock_;
    ResourceManagerMock* resourceManagerMock_;
    Projection projection_;
    FrameBufferMock frameBufferMock_;
    ShadowFrameBufferMock shadowFrameBufferMock_;
    GameEngine::Scene scene_;
    Model model_;
    Mesh mesh_;
    mat4 transformToShader_;
    RendererContext context_;
    std::unique_ptr<EntityRenderer> sut_;
};

TEST_F(EntityRendererShould, RenderWithoutObjectsTest)
{
    ExpectShaderInit();
    sut_->Init();
    AddGameObject();
    ExpectRender();
    sut_->Render(scene_, time_);
}

}  // namespace UT
}  // namespace GameEngine
