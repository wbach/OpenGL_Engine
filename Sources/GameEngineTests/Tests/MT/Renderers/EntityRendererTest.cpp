#include <gtest/gtest.h>
#include <functional>
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Renderers/Objects/Entity/EntityRenderer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"
#include "GameEngineTests/Tests/Mocks/Renderers/FrameBuffer/FrameBufferMock.h"
#include "GameEngineTests/Tests/Mocks/Renderers/Objects/Shadows/ShadowFrameBufferMock.hpp"
#include "GameEngineTests/Tests/Mocks/Resources/ResourcesManagerMock.h"
#include "GameEngineTests/Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "GameEngineTests/Tests/Mocks/Shaders/ShaderFactoryMock.h"
#include "GameEngineTests/Tests/Mocks/Shaders/ShaderProgramMock.h"
#include "GameEngine/Renderers/Objects/Entity/EntityRendererDef.h"
#include "GameEngine/Renderers/Objects/Entity/Shaders/EntityShaderUniforms.h"
#include "GameEngineTests/Tests/UT/Components/BaseComponent.h"

using namespace testing;

namespace GameEngine
{
namespace MT
{
const uint32 SHADER_PROGRAM_ID = 12;
const mat4 VIEW_MATRIX;
const vec3 MATERIAL_AMBIENT{0.f, 0.f, 1.f};
const vec3 MATERIAL_DIFFUSE{1.f, 0.f, 1.f };
const vec3 MATERIAL_SPECULAR{1.f, 1.f, 1.f };

struct EntityRendererShould : public BaseComponentTestSchould
{
    EntityRendererShould()
        : graphicsMock_(std::make_shared<GraphicsApiMock>())
        , shaderFactoryMock_(std::make_shared<ShaderFactoryMock>())
        , frameBufferMock_(std::make_shared<FrameBufferMock>())
        , shadowFrameBufferMock_(std::make_shared<ShadowFrameBufferMock>())
        , context_(&projection_, graphicsMock_, frameBufferMock_, shadowFrameBufferMock_, *shaderFactoryMock_,
                   std::bind(&EntityRendererShould::RenderFunction, this, std::placeholders::_1, std::placeholders::_2))
        , scene_("testScene")
        , mesh_(graphicsMock_)
    {
    }
    void SetUp()
    {
        scene_.SetCamera(std::move(camera_));
        resourceManagerMock_ = new ResourceManagerMock();
        scene_.CreateResourceManger(resourceManagerMock_);
        context_.projection_->CreateProjectionMatrix();

        shaderProgramMock_ = new ShaderProgramMock();
        EXPECT_CALL(*shaderFactoryMock_, createImpl(Shaders::Entity)).WillOnce(Return(shaderProgramMock_));

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
        EXPECT_CALL(*shaderProgramMock_, Init());
        EXPECT_CALL(*shaderProgramMock_, Start()).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::ViewDistance, Matcher<float>(EntityRendererDef::DEFAULT_VIEW_DISTANCE))).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::ShadowVariables, EntityRendererDef::DEFAULT_SHADOW_VARIABLES)).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::ClipPlane, EntityRendererDef::DEFAULT_CLIP_PLANE)).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::IsUseFakeLighting, Matcher<bool>(false))).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::ProjectionMatrix, context_.projection_->GetProjectionMatrix())).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Stop()).Times(1);
    }

    void ExpectRender()
    {
        EXPECT_CALL(*frameBufferMock_, BindToDraw()).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Start()).Times(1);

        EXPECT_CALL(*cameraMock_, GetViewMatrix()).WillOnce(ReturnRef(VIEW_MATRIX));
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::ViewMatrix, VIEW_MATRIX)).Times(1);

        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::ModelMaterialAmbient, MATERIAL_AMBIENT)).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::ModelMaterialDiffuse, MATERIAL_DIFFUSE)).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::ModelMaterialSpecular, MATERIAL_SPECULAR)).Times(1);

        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::UseTexture, Matcher<bool>(false))).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::IsUseNormalMap, Matcher<bool>(false))).Times(1);


        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::TransformationMatrix, transformToShader_)).Times(1);
        EXPECT_CALL(*shaderProgramMock_, Load(EntityShaderUniforms::UseBoneTransform, Matcher<bool>(false))).Times(1);

        EXPECT_CALL(*graphicsMock_, RenderMesh(_)).Times(1);

        EXPECT_CALL(*shaderProgramMock_, Stop()).Times(1);
        EXPECT_CALL(*frameBufferMock_, UnBind()).Times(1);
    }

    void AddGameObject()
    {
        CreateModel();

        EXPECT_CALL(*resourceManagerMock_, GetGpuResourceLoader()).WillOnce(ReturnRef(gpuResourceLoaderMock_));
        EXPECT_CALL(gpuResourceLoaderMock_, AddObjectToGpuLoadingPass(_)).Times(1);
        EXPECT_CALL(*resourceManagerMock_, LoadModel(_)).WillOnce(Return(&model_));

        auto entity = scene_.CreateGameObject();
        entity->AddComponent<Components::RendererComponent>().AddModel("Meshes/sphere.obj");
        sut_->Subscribe(entity.get());
        scene_.AddGameObject(entity);

        transformToShader_ = entity->worldTransform.GetMatrix() * mesh_.GetMeshTransform();
    }

    void CreateModel()
    {
        Material m;
        m.ambient = MATERIAL_AMBIENT;
        m.diffuse = MATERIAL_DIFFUSE;
        m.specular = MATERIAL_SPECULAR;
        mesh_.SetMaterial(m);
        model_.AddMesh(mesh_);
    }
    GpuResourceLoaderMock gpuResourceLoaderMock_;
    std::shared_ptr<GraphicsApiMock> graphicsMock_;
    ShaderProgramMock* shaderProgramMock_;
    std::shared_ptr<ShaderFactoryMock> shaderFactoryMock_;
    ResourceManagerMock* resourceManagerMock_;
    Projection projection_;
    std::shared_ptr<FrameBufferMock> frameBufferMock_;
    std::shared_ptr<ShadowFrameBufferMock> shadowFrameBufferMock_;
    RendererContext context_;
    GameEngine::Scene scene_;
    Model model_;
    Mesh mesh_;
    mat4 transformToShader_;

    std::unique_ptr<EntityRenderer> sut_;
};

TEST_F(EntityRendererShould, RenderWithoutObjectsTest)
{
    ExpectShaderInit();
    sut_->Init();
    AddGameObject();
    ExpectRender();
    sut_->Render(&scene_);
}

}  // namespace MT
}  // namespace GameEngine
