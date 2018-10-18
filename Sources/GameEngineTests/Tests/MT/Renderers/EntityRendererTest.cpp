#include <gtest/gtest.h>
#include <functional>
#include "GameEngine/Renderers/Objects/Entity/EntityRenderer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Renderers/FrameBuffer/FrameBufferMock.h"
#include "GameEngineTests/Tests/Mocks/Renderers/Objects/Shadows/ShadowFrameBufferMock.hpp"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"

using namespace testing;

namespace GameEngine
{
namespace MT
{
const uint32 SHADER_PROGRAM_ID = 12;
const mat4 VIEW_MATRIX;

struct EntityRendererShould : public ::testing::Test
{
    EntityRendererShould()
        : graphicsMock_(std::make_shared<GraphicsApiMock>())
        , frameBufferMock_(std::make_shared<FrameBufferMock>())
        , shadowFrameBufferMock_(std::make_shared<ShadowFrameBufferMock>())
        , context_(&projection_, graphicsMock_, frameBufferMock_, shadowFrameBufferMock_,
                   std::bind(&EntityRendererShould::RenderFunction, this, std::placeholders::_1, std::placeholders::_2))
		, scene_("testScene")
		, cameraMock_(std::make_shared<CameraMock>())
    {
    }
    void SetUp()
    {
		scene_.SetCamera(cameraMock_);
		context_.projection_->CreateProjectionMatrix();
        sut_.reset(new EntityRenderer(context_));
    }
    void RenderFunction(RendererFunctionType, RendererFunction)
    {
    }
    void ExpectCalls()
    {
        EXPECT_CALL(*graphicsMock_, DeleteObject(0)).Times(1);
    }
    void ExpectShaderInit()
    {
        EXPECT_CALL(*graphicsMock_, CreateShader(_, _)).WillOnce(Return(SHADER_PROGRAM_ID));
        EXPECT_CALL(*graphicsMock_, DeleteObject(SHADER_PROGRAM_ID)).Times(1);
        EXPECT_CALL(*graphicsMock_, UseShader(SHADER_PROGRAM_ID)).Times(1);
		EXPECT_CALL(*graphicsMock_, LoadValueToShader(0, Matcher<float>(500.f))).Times(1);
		EXPECT_CALL(*graphicsMock_, LoadValueToShader(0, vec4(0.f, 1.f, 0.f, 100000.f))).Times(1);
		EXPECT_CALL(*graphicsMock_, LoadValueToShader(0, vec3(0.f, 5.f, 10.f))).Times(1);
		EXPECT_CALL(*graphicsMock_, LoadValueToShader(0, Matcher<float>(0.f))).Times(1);
		EXPECT_CALL(*graphicsMock_, LoadValueToShader(0, context_.projection_->GetProjectionMatrix())).Times(1);
		EXPECT_CALL(*graphicsMock_, UseShader(0)).Times(1);
		
    }
	void ExpectShaderRender()
	{
		EXPECT_CALL(*frameBufferMock_, BindToDraw()).Times(1);
		EXPECT_CALL(*graphicsMock_, UseShader(SHADER_PROGRAM_ID)).Times(1);
		EXPECT_CALL(*graphicsMock_, UseShader(0)).Times(1);
		EXPECT_CALL(*frameBufferMock_, UnBind()).Times(1);

		EXPECT_CALL(*cameraMock_, GetViewMatrix()).WillOnce(ReturnRef(VIEW_MATRIX));
		EXPECT_CALL(*graphicsMock_, LoadValueToShader(0, VIEW_MATRIX)).Times(1);
		//shader.LoadViewMatrix(scene_.GetCamera()->GetViewMatrix());
		// 	shader.LoadUseBonesTransformation(0.f);
		EXPECT_CALL(*graphicsMock_, LoadValueToShader(0, Matcher<float>(0.f))).Times(1);

	}
    std::shared_ptr<GraphicsApiMock> graphicsMock_;
    CProjection projection_;
    std::shared_ptr<FrameBufferMock> frameBufferMock_;
    std::shared_ptr<ShadowFrameBufferMock> shadowFrameBufferMock_;
    RendererContext context_;
	GameEngine::Scene scene_;
	std::shared_ptr<CameraMock> cameraMock_;
    std::unique_ptr<EntityRenderer> sut_;
};

TEST_F(EntityRendererShould, deleteShaderProgramOnEnd)
{
    EXPECT_CALL(*graphicsMock_, DeleteObject(0)).Times(1);
}

TEST_F(EntityRendererShould, t)
{
    ExpectShaderInit();
    sut_->Init();
	ExpectShaderRender();
	sut_->Render(&scene_);
}

}  // MT
}  // GameEngine
