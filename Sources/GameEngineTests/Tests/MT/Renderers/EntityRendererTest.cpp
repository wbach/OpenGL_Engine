#include <gtest/gtest.h>
#include <functional>
#include "GameEngine/Renderers/Objects/Entity/EntityRenderer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Renderers/FrameBuffer/FrameBufferMock.h"
#include "GameEngineTests/Tests/Mocks/Renderers/Objects/Shadows/ShadowFrameBufferMock.hpp"

using namespace testing;

namespace GameEngine
{
namespace MT
{
const uint32 SHADER_PROGRAM_ID = 12;
struct EntityRendererShould : public ::testing::Test
{
    EntityRendererShould()
        : graphicsMock_(std::make_shared<GraphicsApiMock>())
        , frameBufferMock_(std::make_shared<FrameBufferMock>())
        , shadowFrameBufferMock_(std::make_shared<ShadowFrameBufferMock>())
        , context_(&projection_, graphicsMock_, frameBufferMock_, shadowFrameBufferMock_,
                   std::bind(&EntityRendererShould::RenderFunction, this, std::placeholders::_1, std::placeholders::_2))
    {
    }
    void SetUp()
    {
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
    }
    std::shared_ptr<GraphicsApiMock> graphicsMock_;
    CProjection projection_;
    std::shared_ptr<FrameBufferMock> frameBufferMock_;
    std::shared_ptr<ShadowFrameBufferMock> shadowFrameBufferMock_;
    RendererContext context_;
    std::unique_ptr<EntityRenderer> sut_;
};

TEST_F(EntityRendererShould, deleteShaderProgramOnEnd)
{
    EXPECT_CALL(*graphicsMock_, DeleteObject(0)).Times(1);
}

//TEST_F(EntityRendererShould, t)
//{
//    ExpectShaderInit();
//    sut_->Init();
//}

}  // MT
}  // GameEngine
