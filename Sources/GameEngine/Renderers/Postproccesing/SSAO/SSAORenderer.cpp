#include "SSAORenderer.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <GraphicsApi/TextureParamters.h>
#include <Logger/Log.h>
#include <Utils/Image/Image.h>

#include <glm/glm.hpp>
#include <random>
#include <vector>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "Types.h"

namespace GameEngine
{
namespace
{
struct SsaoBuffer
{
    mat4 projection;
    AlignWrapper<vec4> params{vec4{1.f}};  // xy - noiseScale, z - radius, w - bias
    AlignWrapper<vec4> samples[64];
};
}  // namespace

SSAORenderer::SSAORenderer(RendererContext &context)
    : context(context)
    , ssaoShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SSAO)
    , blurShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SsaoBlure)
    , isEnabled(EngineConf.renderer.ssao.isEnabled)
{
    enabledSubId =
        EngineConf.renderer.ssao.isEnabled.subscribeForChange([&]() { isEnabled = EngineConf.renderer.ssao.isEnabled; });
}
SSAORenderer::~SSAORenderer()
{
    if (enabledSubId)
    {
        EngineConf.renderer.ssao.isEnabled.unsubscribe(*enabledSubId);
        enabledSubId.reset();
    }
}
void SSAORenderer::Init()
{
    initStatus = InitStatus::ok;

    if (not context.camera_)
    {
        LOG_ERROR << "Camera not set during intialization!";
        initStatus = InitStatus::failure;
        return;
    }

    const auto &camera        = *context.camera_;
    const auto &renderingSize = camera.GetProjection().GetRenderingSize();
    // float width               = static_cast<float>(renderingSize.x) / EngineConf.renderer.ssao.resolutionDevider.get();
    // float height              = static_cast<float>(renderingSize.y) / EngineConf.renderer.ssao.resolutionDevider.get();
    frameBufferSize = renderingSize;  // TO DO:vec2ui(static_cast<uint32>(width), static_cast<uint32>(height));

    blurShader_.Init();

    if (not blurShader_.IsReady())
    {
        initStatus = InitStatus::failure;
        return;
    }

    ssaoShader_.Init();

    if (not ssaoShader_.IsReady())
    {
        initStatus = InitStatus::failure;
        return;
    }

    GenKernel();
    GenerateNoiseTexture();
    CreateFbos();
    CreateUniformBuffer();

    LOG_DEBUG << "Init done.";
}
void SSAORenderer::CleanUp()
{
    if (ssaFbo)
    {
        context.graphicsApi_.DeleteFrameBuffer(*ssaFbo);
        ssaFbo = nullptr;
    }

    if (ssaBlurFbo)
    {
        context.graphicsApi_.DeleteFrameBuffer(*ssaBlurFbo);
        ssaBlurFbo = nullptr;
    }

    if (noiseTextureId)
    {
        context.graphicsApi_.DeleteObject(*noiseTextureId);
        noiseTextureId.reset();
    }

    kernel.clear();
    blurShader_.Clear();
    ssaoShader_.Clear();

    if (ubuffer)
    {
        context.graphicsApi_.DeleteShaderBuffer(*ubuffer);
        ubuffer.reset();
    }

    context.sharedTextures[magic_enum::enum_index(SharedTextures::ssao).value()].reset();
    initStatus.reset();

    LOG_DEBUG << "CleanUp done.";
}
void SSAORenderer::Render(uint32 depthTextureId, uint32 normalTextureId)
{
    if (isEnabled and not initStatus)
    {
        LOG_DEBUG << "Enabling ssao renderer";
        Init();
    }
    else if (not isEnabled and initStatus == InitStatus::ok)
    {
        LOG_DEBUG << "Disabling ssao renderer";
        CleanUp();
    }

    if (not initStatus or initStatus == InitStatus::failure)
    {
        return;
    }

    SSAOPass(depthTextureId, normalTextureId);
    BlurPass();
}
void SSAORenderer::ReloadShaders()
{
    ssaoShader_.Reload();
    blurShader_.Reload();
}
void SSAORenderer::SSAOPass(uint32 depthTextureId, uint32 normalTextureId)
{
    if (not noiseTextureId)
    {
        LOG_WARN << "noise texture not ready";
        return;
    }

    ssaFbo->Clear();
    ssaFbo->Bind(GraphicsApi::FrameBuffer::BindType::Write);
    ssaoShader_.Start();
    context.graphicsApi_.BindShaderBuffer(*ubuffer);
    context.graphicsApi_.ActiveTexture(0, depthTextureId);
    context.graphicsApi_.ActiveTexture(1, normalTextureId);
    context.graphicsApi_.ActiveTexture(2, *noiseTextureId);
    context.graphicsApi_.RenderProcedural(3);
    ssaoShader_.Stop();
}
void SSAORenderer::BlurPass()
{
    if (not ssaColorTextureId)
    {
        LOG_WARN << "ssaColorTexture not ready";
        return;
    }

    ssaBlurFbo->Clear();
    ssaBlurFbo->Bind(GraphicsApi::FrameBuffer::BindType::Write);
    blurShader_.Start();
    context.graphicsApi_.ActiveTexture(0, *ssaColorTextureId);
    context.graphicsApi_.RenderProcedural(3);
    blurShader_.Stop();
    ssaBlurFbo->UnBind();
}
void SSAORenderer::GenKernel()
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    kernel.clear();
    kernel.reserve(kernelSize);
    for (int i = 0; i < kernelSize; ++i)
    {
        vec3 sample(randomFloats(generator) * 2.0 - 1.0,  // X: -1.0 do 1.0
                    randomFloats(generator) * 2.0 - 1.0,  // Y: -1.0 do 1.0
                    randomFloats(generator)               // Z:  0.0 do 1.0
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = (float)i / (float)kernelSize;
        scale       = glm::mix(0.1f, 1.0f, scale * scale);
        sample *= scale;

        kernel.push_back(sample);
    }
}
void SSAORenderer::GenerateNoiseTexture()
{
    Utils::Image noiseImage;
    noiseImage.width     = 4;
    noiseImage.height    = 4;
    noiseImage.channels_ = 3;
    noiseImage.allocateImage<float>();

    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for (uint32 y = 0; y < noiseImage.height; y++)
    {
        for (uint32 x = 0; x < noiseImage.width; x++)
        {
            vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
            noiseImage.setPixel(vec2ui(x, y), Color(noise));
        }
    }

    // GL_REPEAT is by default
    noiseTextureId =
        context.graphicsApi_.CreateTexture(noiseImage, GraphicsApi::TextureFilter::NEAREST, GraphicsApi::TextureMipmap::NONE);

    if (not noiseTextureId)
    {
        LOG_WARN << "Noise texture creation error";
        initStatus = InitStatus::failure;
    }
}
void SSAORenderer::CreateFbos()
{
    if (not ssaFbo)
    {
        ssaFbo = createFrameBuffer();

        if (ssaFbo)
        {
            ssaColorTextureId = ssaFbo->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
        }
        else
        {
            initStatus = InitStatus::failure;
        }
    }

    if (not ssaBlurFbo)
    {
        ssaBlurFbo = createFrameBuffer();

        if (ssaBlurFbo)
        {
            auto atachmentId = ssaBlurFbo->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
            context.sharedTextures[magic_enum::enum_index(SharedTextures::ssao).value()] = atachmentId;
        }
        else
        {
            initStatus = InitStatus::failure;
        }
    }
}
GraphicsApi::IFrameBuffer *SSAORenderer::createFrameBuffer()
{
    using namespace GraphicsApi::FrameBuffer;

    Attachment color(*frameBufferSize, Type::Color0, Format::R32f);
    color.filter = GraphicsApi::FrameBuffer::Filter::Linear;

    auto frameBuffer = &context.graphicsApi_.CreateFrameBuffer({color});
    if (not frameBuffer->Init())
    {
        LOG_DEBUG << "Framebuffer creation error";
        context.graphicsApi_.DeleteFrameBuffer(*frameBuffer);
        return nullptr;
    }

    return frameBuffer;
}
void SSAORenderer::CreateUniformBuffer()
{
    if (not ubuffer)
    {
        const uint32 BIND_LOCATION{6};
        ubuffer = context.graphicsApi_.CreateShaderBuffer(BIND_LOCATION, sizeof(SsaoBuffer), GraphicsApi::DrawFlag::Dynamic);

        if (ubuffer)
        {
            SsaoBuffer buffer;
            const float radius = EngineConf.renderer.ssao.radius;
            const float bias   = EngineConf.renderer.ssao.bias;
            const auto &camera = *context.camera_;
            buffer.projection  = camera.GetProjection().GetMatrix();
            buffer.params      = vec4((float)frameBufferSize->x / 4.0f, (float)frameBufferSize->y / 4.0f, radius, bias);

            for (int i = 0; i < kernelSize; ++i)
            {
                buffer.samples[i] = glm::vec4(kernel[i], 0.0f);
            }

            context.graphicsApi_.UpdateShaderBuffer(*ubuffer, &buffer);
        }
        else
        {
            initStatus = InitStatus::failure;
        }
    }
}
}  // namespace GameEngine
