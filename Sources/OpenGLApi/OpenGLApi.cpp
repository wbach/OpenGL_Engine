#include "OpenGLApi.h"

#include <GL/glew.h>
#include <Utils/Image/ImageUtils.h>
#include <Utils/Variant.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>

#include "Font.h"
#include "GameEngine/Engine/Configuration.h"
#include "GlFrameBuffer.h"
#include "GraphicsApi/MeshRawData.h"
#include "GraphicsApi/TextureInfo.h"
#include "IdPool.h"
#include "Logger/Log.h"
#include "OpenGLUtils.h"
#include "SDL2/SDLOpenGL.h"

enum class ObjectType
{
    TEXTURE_2D,
    TEXTURE_CUBE_MAP,
    FRAME_BUFFER,
    RENDER_BUFFER,
    SHADER_PROGRAM,
    MESH
};

namespace OpenGLApi
{
namespace
{
std::unordered_map<uint32, ObjectType> createdObjectIds;
std::unordered_map<uint32, int64> allocatedTextureBytes;
std::unordered_map<GraphicsApi::ShaderType, uint32> shaderTypeMap_;
std::unordered_map<GraphicsApi::TextureFilter, GLfloat> textureFilterMap_;
std::unordered_map<GraphicsApi::TextureAccess, uint32> textureAccessMap_;
std::unordered_map<GraphicsApi::RenderType, uint32> renderTypeMap_;
CFont font;
const GraphicsApi::TextureInfo defaultTextureInfo;

struct TextTypeParams
{
    GLenum target        = GL_TEXTURE_2D;
    GLenum dataType      = GL_FLOAT;
    GLint internalFormat = GL_RGBA;
    GLenum format        = GL_RGBA;
};

TextTypeParams GetTextureTypeParams(GraphicsApi::TextureType type)
{
    TextTypeParams params;

    if (type == GraphicsApi::TextureType::FLOAT_BUFFER_2D || type == GraphicsApi::TextureType::FLOAT_TEXTURE_2D ||
        type == GraphicsApi::TextureType::DEPTH_BUFFER_2D)
    {
        params.dataType = GL_FLOAT;
        if (type == GraphicsApi::TextureType::DEPTH_BUFFER_2D)
        {
            params.internalFormat = GL_DEPTH_COMPONENT32;
            params.format         = GL_DEPTH_COMPONENT;
        }
        else
        {
            params.internalFormat = GL_RGBA16F;
        }
    }
    else if (type == GraphicsApi::TextureType::FLOAT_TEXTURE_1D)
    {
        params.format         = GL_RED;
        params.dataType       = GL_FLOAT;
        params.internalFormat = GL_R32F;
    }
    else if (type == GraphicsApi::TextureType::U8_RGBA)
    {
        params.format         = GL_RGBA;
        params.dataType       = GL_UNSIGNED_BYTE;
        params.internalFormat = GL_RGBA;
    }
    else if (type == GraphicsApi::TextureType::FLOAT_TEXTURE_3D)
    {
        params.format         = GL_RGB;
        params.dataType       = GL_FLOAT;
        params.internalFormat = GL_RGB32F;
    }
    else if (type == GraphicsApi::TextureType::FLOAT_TEXTURE_4D)
    {
        params.format         = GL_RGBA;
        params.dataType       = GL_FLOAT;
        params.internalFormat = GL_RGBA32F;
    }
    return params;
}

}  // namespace

struct ShaderBuffer
{
    uint32 glId;
    bool isInGpu;
    uint32 bufferSize;
    uint32 bindLocation;
};

struct DebugNormalMesh
{
    std::vector<float> position_;
    std::vector<float> normal_;
    std::vector<float> tangent_;
    GraphicsApi::ID id_;
};

struct OpenGLApi::Pimpl
{
    Pimpl()
        : shaderManager_(idPool_)
    {
    }
    GLint maxPatchVertices_ = 0;
    IdPool idPool_;
    ShaderManager shaderManager_;
    std::vector<ShaderBuffer> shaderBuffers_;
    std::unordered_map<uint32, GraphicsApi::TextureInfo> textureInfos_;
    std::vector<std::unique_ptr<FrameBuffer>> frameBuffers_;

    std::unordered_map<uint64, ObjectType> createdGraphicsObjects_;

    std::unordered_map<uint32, DebugNormalMesh> debugNormalsMesh_;
};

OpenGLMesh Convert(const Vao& v)
{
    OpenGLMesh mesh;
    mesh.vao          = v.vao;
    mesh.vbos         = v.vbos;
    mesh.attributes   = v.attributes;
    mesh.vertexCount  = static_cast<GLsizei>(v.size);
    mesh.useIndiecies = v.useIndicies;
    mesh.sizeInBytes  = v.sizeInBytes;
    return mesh;
}
OpenGLApi::OpenGLApi()
    : OpenGLApi(std::make_unique<SdlOpenGlApi>())
{
}
OpenGLApi::OpenGLApi(std::unique_ptr<GraphicsApi::IWindowApi> windowApi)
    : windowApi_(std::move(windowApi))
    , bgColor_(0)
    , quad_()
    , quadTs_(true)
    , polygonMode_(true)
    , useLowGLversion_(false)
{
    impl_ = std::make_unique<Pimpl>();

    textureFilterMap_ = {{GraphicsApi::TextureFilter::LINEAR, GL_LINEAR},
                         {GraphicsApi::TextureFilter::NEAREST, GL_NEAREST}};

    renderTypeMap_ = {{GraphicsApi::RenderType::PATCHES, GL_PATCHES},
                      {GraphicsApi::RenderType::POINTS, GL_POINTS},
                      {GraphicsApi::RenderType::LINES, GL_LINES},
                      {GraphicsApi::RenderType::TRIAGNLE_STRIP, GL_TRIANGLE_STRIP},
                      {GraphicsApi::RenderType::TRIANGLES, GL_TRIANGLES}};

    textureAccessMap_ = {{GraphicsApi::TextureAccess::WRITE_ONLY, GL_WRITE_ONLY}};

    for (auto& b : bindedShaderBuffers_)
    {
        b = 0;
    }
}

OpenGLApi::~OpenGLApi()
{
}

void GetInfoAndPrint(const std::string& str, GLenum i)
{
    int info{0};
    glGetIntegerv(i, &info);
    INFO_LOG(str + " : " + std::to_string(info));
}

void OpenGLApi::Init()
{
    DEBUG_LOG("Init openGLApi");

    auto glew_init_result = glewInit();

    if (glew_init_result != GLEW_OK)
    {
        std::string err(reinterpret_cast<char const*>(glewGetErrorString(glew_init_result)));
        ERROR_LOG("Glew init error : " + err);
        return;
    }

    quad_.Init();
    quadTs_.Init();

    glGetIntegerv(GL_MAJOR_VERSION, &openGLVersion_.x);
    glGetIntegerv(GL_MINOR_VERSION, &openGLVersion_.y);

    if (openGLVersion_.x < 4)
    {
        useLowGLversion_ = true;
        impl_->shaderManager_.UseDeprectedShaders();
    }

    glPolygonOffset(1, 1);

    DEBUG_LOG("Init done.");
}
void OpenGLApi::SetShadersFilesLocations(const std::string& path)
{
    impl_->shaderManager_.SetShadersFilesLocations(path);
}
void OpenGLApi::SetShaderQuaility(GraphicsApi::ShaderQuaility q)
{
    impl_->shaderManager_.SetShaderQuaility(q);
}
void OpenGLApi::CreateContext()
{
    windowApi_->CreateContext();
}
void OpenGLApi::DeleteContext()
{
    ClearRest();
    windowApi_->DeleteContext();
}
void OpenGLApi::PrintVersion()
{
    std::string ver(reinterpret_cast<char const*>(glGetString(GL_VERSION)));
    INFO_LOG("GL version: " + ver);

    std::string glslver(reinterpret_cast<char const*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    INFO_LOG("GLSL version: " + glslver);

    if (not useLowGLversion_)
    {
        glGetIntegerv(GL_MAX_PATCH_VERTICES, &impl_->maxPatchVertices_);
        INFO_LOG("Max supported patch vertices :" + std::to_string(impl_->maxPatchVertices_));

        if (impl_->maxPatchVertices_)
        {
            glPatchParameteri(GL_PATCH_VERTICES, 3);
        }
    }

    GetInfoAndPrint("GL_MAX_GEOMETRY_UNIFORM_BLOCKS", GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
    GetInfoAndPrint("GL_MAX_GEOMETRY_SHADER_INVOCATIONS", GL_MAX_GEOMETRY_SHADER_INVOCATIONS);
    GetInfoAndPrint("GL_MAX_UNIFORM_BUFFER_BINDINGS", GL_MAX_UNIFORM_BUFFER_BINDINGS);
    GetInfoAndPrint("GL_MAX_UNIFORM_BLOCK_SIZE", GL_MAX_UNIFORM_BLOCK_SIZE);
    GetInfoAndPrint("GL_MAX_SHADER_STORAGE_BLOCK_SIZE", GL_MAX_SHADER_STORAGE_BLOCK_SIZE);
    GetInfoAndPrint("GL_MAX_TEXTURE_SIZE", GL_MAX_TEXTURE_SIZE);
    GetInfoAndPrint("GL_MAX_TEXTURE_BUFFER_SIZE", GL_MAX_TEXTURE_BUFFER_SIZE);
    GetInfoAndPrint("GL_ACTIVE_ATTRIBUTE_MAX_LENGTH", GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
    GetInfoAndPrint("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    GetInfoAndPrint("GL_MAX_COLOR_ATTACHMENTS", GL_MAX_COLOR_ATTACHMENTS);
    GetInfoAndPrint("GL_MAX_COLOR_TEXTURE_SAMPLES", GL_MAX_COLOR_TEXTURE_SAMPLES);
}

std::vector<GraphicsApi::RendererType> OpenGLApi::GetSupportedRenderers() const
{
    return {GraphicsApi::RendererType::SIMPLE, GraphicsApi::RendererType::FULL};
}

bool OpenGLApi::IsTesselationSupported() const
{
    return impl_->maxPatchVertices_ > 0;
}
GraphicsApi::IWindowApi& OpenGLApi::GetWindowApi()
{
    return *windowApi_;
}
void OpenGLApi::PrepareFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bgColor_.value.x, bgColor_.value.y, bgColor_.value.z, bgColor_.value.w);

    auto errorString = GetGlError();

    if (not errorString.empty())
    {
        ERROR_LOG(errorString);
    }
}
void OpenGLApi::SetDefaultTarget()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
void OpenGLApi::SetBackgroundColor(const Color& color)
{
    bgColor_ = color;
}

const Color& OpenGLApi::GetBackgroundColor() const
{
    return bgColor_;
}
void OpenGLApi::EnableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
}
void OpenGLApi::DisableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
}
GraphicsApi::ID OpenGLApi::CreateShader(GraphicsApi::ShaderProgramType shaderType)
{
    auto id = impl_->shaderManager_.Create(shaderType);

    if (not id)
        return {};

    createdObjectIds.insert({*id, ObjectType::SHADER_PROGRAM});
    return id;
}
void OpenGLApi::DeleteShader(uint32 id)
{
    impl_->shaderManager_.DeleteShader(id);
}
void OpenGLApi::ClearRest()
{
    for (auto& buffer : impl_->shaderBuffers_)
    {
        if (buffer.isInGpu)
            glDeleteBuffers(1, &buffer.glId);
    }

    impl_->shaderBuffers_.clear();

    std::vector<uint32> objectsToRemove;
    for (auto iter = createdObjectIds.begin(); iter != createdObjectIds.end(); iter++)
    {
        objectsToRemove.push_back(iter->first);
    }

    for (auto& id : objectsToRemove)
    {
        DeleteObject(id);
    }

    createdObjectIds.clear();
}
void OpenGLApi::LoadViewMatrix(const mat4& viewMatrix)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&viewMatrix[0][0]);
}
void OpenGLApi::LoadProjectionMatrix(const mat4& projectionMatrix)
{
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(&projectionMatrix[0][0]);
}
void OpenGLApi::DrawLine(const vec3& color, const vec3& from, const vec3& to)
{
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_LINES);
    glVertex3f(from.x, from.y, from.z);
    glVertex3f(to.x, to.y, to.z);
    glEnd();
}

mat4 OpenGLApi::PrepareMatrixToLoad(const mat4& m)
{
    return m;
}

std::vector<uint8> OpenGLApi::GetTextureData(uint32 id) const
{
    auto iter = impl_->textureInfos_.find(id);

    if (iter == impl_->textureInfos_.end())
    {
        return {};
    }

    const auto& textureInfo = iter->second;
    auto params             = GetTextureTypeParams(textureInfo.textureType);

    std::vector<uint8> data;
    BindTexture(id);

    if (params.dataType == GL_UNSIGNED_BYTE and params.format == GL_RGBA)
    {
        data.resize(4 * textureInfo.size.x * textureInfo.size.y);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    }
    else if (params.dataType == GL_FLOAT and params.format == GL_RGBA)
    {
        data.resize(4 * textureInfo.size.x * textureInfo.size.y);
        std::vector<float> floatdata;
        floatdata.resize(4 * textureInfo.size.x * textureInfo.size.y);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &floatdata[0]);

        std::transform(floatdata.begin(), floatdata.end(), data.begin(),
                       [](float f) { return static_cast<uint8>(255.f * f); });
    }
    else if (params.dataType == GL_FLOAT and params.format == GL_RGB)
    {
        std::vector<float> floatdata;
        data.reserve(4 * textureInfo.size.x * textureInfo.size.y);
        floatdata.resize(3 * textureInfo.size.x * textureInfo.size.y);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &floatdata[0]);

        for (size_t i = 0; i < floatdata.size(); i += 3)
        {
            auto x = static_cast<uint8>(255.f * floatdata[i]);
            auto y = static_cast<uint8>(255.f * floatdata[i + 1]);
            auto z = static_cast<uint8>(255.f * floatdata[i + 2]);

            data.push_back(x);
            data.push_back(y);
            data.push_back(z);
            data.push_back(255);
        }
    }
    else if (params.dataType == GL_FLOAT and params.format == GL_RED)
    {
        data.reserve(4 * textureInfo.size.x * textureInfo.size.y);
        std::vector<float> floatdata;
        floatdata.resize(textureInfo.size.x * textureInfo.size.y);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, &floatdata[0]);

        for (auto& f : floatdata)
        {
            auto value = static_cast<uint8>(255.f * f);
            data.push_back(value);
            data.push_back(value);
            data.push_back(value);
            data.push_back(255);
        }
    }
    return data;
}

const GraphicsApi::TextureInfo& OpenGLApi::GetTextureInfo(uint32 id) const
{
    auto iter = impl_->textureInfos_.find(id);
    if (iter != impl_->textureInfos_.end())
        return iter->second;

    return defaultTextureInfo;
}

void OpenGLApi::TakeSnapshoot(const std::string& path) const
{
    for (const auto& object : createdObjectIds)
    {
        switch (object.second)
        {
            case ObjectType::TEXTURE_2D:
            {
                if (impl_->textureInfos_.count(object.first) == 0)
                {
                    DEBUG_LOG("Texture info not found. Id : " + std::to_string(object.first));
                    break;
                }
                const auto& textureInfo    = GetTextureInfo(object.first);
                auto resultData            = GetTextureData(object.first);
                const std::string fullPath = path + "/Textures2d/";
                std::filesystem::create_directories(fullPath);
                Utils::SaveImage(resultData, textureInfo.size, fullPath + std::to_string(object.first));
            }
            break;
            default:
                DEBUG_LOG("not imeplmented");
                break;
        }
    }
    // moving to graphics objects
    for (const auto& object : impl_->createdGraphicsObjects_)
    {
        switch (object.second)
        {
            case ObjectType::FRAME_BUFFER:
            {
                const auto& frameBuffer = impl_->frameBuffers_.at(object.first);

                const std::string fullPath =
                    path + "/FrameBuffersTextures/FrameBuffer_" + std::to_string(frameBuffer->GetId()) + "/";
                std::filesystem::create_directories(fullPath);
                impl_->frameBuffers_.at(object.first)->TakeSnapshot(fullPath);
            }
            break;
            default:
                DEBUG_LOG("not imeplmented");
                break;
        }
    }
}

void OpenGLApi::BindDefaultFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLApi::IFrameBuffer& OpenGLApi::CreateFrameBuffer(
    const std::vector<GraphicsApi::FrameBuffer::Attachment>& attachments)
{
    impl_->frameBuffers_.push_back(std::make_unique<FrameBuffer>(impl_->idPool_, attachments));
    DEBUG_LOG(std::to_string(impl_->frameBuffers_.back()->GetId()));
    impl_->createdGraphicsObjects_.insert({impl_->frameBuffers_.back()->GetId(), ObjectType::FRAME_BUFFER});
    return *impl_->frameBuffers_.back();
}

void OpenGLApi::DeleteFrameBuffer(OpenGLApi::IFrameBuffer& framebuffer)
{
    DEBUG_LOG(std::to_string(framebuffer.GetId()));
    framebuffer.CleanUp();

    auto& v   = impl_->frameBuffers_;
    auto iter = std::find_if(v.begin(), v.end(),
                             [id = framebuffer.GetId()](const auto& object) { return object->GetId() == id; });

    if (iter != v.end())
    {
        v.erase(iter);
    }
}

void OpenGLApi::CreateDebugNormalMesh(uint32 rid, const GraphicsApi::MeshRawData& meshRawData)
{
    if (not meshRawData.positions_.empty() and not meshRawData.normals_.empty())
    {
        if (not impl_->debugNormalsMesh_.count(rid))
        {
            impl_->debugNormalsMesh_.insert({rid, {}});
        }

        auto& debugNormalMesh = impl_->debugNormalsMesh_.at(rid);
        debugNormalMesh.position_.clear();
        debugNormalMesh.normal_.clear();
        debugNormalMesh.tangent_.clear();

        debugNormalMesh.position_.reserve(meshRawData.positions_.size());
        debugNormalMesh.normal_.reserve(meshRawData.positions_.size());
        debugNormalMesh.tangent_.reserve(meshRawData.positions_.size());

        for (size_t i = 0; i < meshRawData.positions_.size(); i += 3)
        {
            auto p1 = meshRawData.positions_[i];
            auto p2 = meshRawData.positions_[i + 1];
            auto p3 = meshRawData.positions_[i + 2];
            auto n1 = meshRawData.normals_[i];
            auto n2 = meshRawData.normals_[i + 1];
            auto n3 = meshRawData.normals_[i + 2];
            auto t1 = meshRawData.tangents_[i];
            auto t2 = meshRawData.tangents_[i + 1];
            auto t3 = meshRawData.tangents_[i + 2];
            debugNormalMesh.position_.push_back(p1);
            debugNormalMesh.position_.push_back(p2);
            debugNormalMesh.position_.push_back(p3);
            debugNormalMesh.normal_.push_back(n1);
            debugNormalMesh.normal_.push_back(n2);
            debugNormalMesh.normal_.push_back(n3);
            debugNormalMesh.tangent_.push_back(t1);
            debugNormalMesh.tangent_.push_back(t2);
            debugNormalMesh.tangent_.push_back(t3);
        }

        if (not debugNormalMesh.id_)
        {
            debugNormalMesh.id_ = impl_->idPool_.ToUint(0);
            createdObjectIds.insert({*debugNormalMesh.id_, ObjectType::MESH});

            openGlMeshes_.insert({*debugNormalMesh.id_, {}});
            auto& mesh = openGlMeshes_.at(*debugNormalMesh.id_);

            VaoCreator vaoCreator;
            vaoCreator.SetSize(0);
            vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::POSITION, 3);
            vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::NORMAL, 3);
            vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::TANGENT, 3);
            mesh            = Convert(vaoCreator.Get());
            mesh.renderType = GraphicsApi::RenderType::POINTS;
        }

        if (debugNormalMesh.id_)
        {
            auto& obj                    = openGlMeshes_.at(*debugNormalMesh.id_);
            auto sizeInBytesBeforeUpdate = obj.sizeInBytes;
            obj.sizeInBytes              = 0;

            obj.vertexCount = static_cast<GLsizei>(debugNormalMesh.position_.size());

            glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::POSITION]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * debugNormalMesh.position_.size(),
                         &debugNormalMesh.position_[0], GL_STREAM_DRAW);
            obj.sizeInBytes += sizeof(float) * debugNormalMesh.position_.size();

            glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::NORMAL]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * debugNormalMesh.normal_.size(), &debugNormalMesh.normal_[0],
                         GL_STREAM_DRAW);
            obj.sizeInBytes += sizeof(float) * debugNormalMesh.normal_.size();

            glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::TANGENT]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * debugNormalMesh.tangent_.size(), &debugNormalMesh.tangent_[0],
                         GL_STREAM_DRAW);
            obj.sizeInBytes += sizeof(float) * debugNormalMesh.tangent_.size();

            allocatedBytes(obj.sizeInBytes - sizeInBytesBeforeUpdate);
        }
    }
}

void OpenGLApi::DeleteMesh(uint32 id)
{
    // DEBUG_LOG("openGlMeshes_ size  " + std::to_string(openGlMeshes_.size()));

    auto iter = openGlMeshes_.find(id);
    if (iter == openGlMeshes_.end())
        return;

    auto& mesh = iter->second;

    for (auto& vbo : mesh.vbos)
    {
        if (vbo.second != 0)
            glDeleteBuffers(1, &vbo.second);
    }
    allocatedBytes(-mesh.sizeInBytes);

    glDeleteVertexArrays(1, &mesh.vao);

    openGlMeshes_.erase(id);
    DeleteDebugNormalMesh(id);
    // DEBUG_LOG("erase openGlMeshes_ size  " + std::to_string(openGlMeshes_.size()));
}

void OpenGLApi::DeleteDebugNormalMesh(uint32 id)
{
    auto iter = impl_->debugNormalsMesh_.find(id);
    if (iter == impl_->debugNormalsMesh_.end())
        return;

    auto& mesh = iter->second;

    if (mesh.id_)
    {
        auto iter = openGlMeshes_.find(*mesh.id_);
        if (iter != openGlMeshes_.end())
        {
            auto& glmesh = iter->second;

            for (auto& vbo : glmesh.vbos)
            {
                if (vbo.second != 0)
                    glDeleteBuffers(1, &vbo.second);
            }

            glDeleteVertexArrays(1, &glmesh.vao);
            openGlMeshes_.erase(id);
        }
    }
    impl_->debugNormalsMesh_.erase(id);
}

void OpenGLApi::UseShader(uint32 id)
{
    impl_->shaderManager_.UseShader(id);
}

GraphicsApi::ID OpenGLApi::CreateShaderBuffer(uint32 bindLocation, uint32 size)
{
    uint32 buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    impl_->shaderBuffers_.push_back({buffer, true, size, bindLocation});
    auto id = impl_->shaderBuffers_.size() - 1;
    return id;
}

void OpenGLApi::UpdateShaderBuffer(uint32 id, void const* buffer)
{
    const auto& b = impl_->shaderBuffers_[id];

    glBindBuffer(GL_UNIFORM_BUFFER, b.glId);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, b.bufferSize, buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

uint32 OpenGLApi::BindShaderBuffer(uint32 id)
{
    const auto& b = impl_->shaderBuffers_[id];
    uint32 result = bindedShaderBuffers_[b.bindLocation];

    bindedShaderBuffers_[b.bindLocation] = id;
    glBindBufferBase(GL_UNIFORM_BUFFER, b.bindLocation, b.glId);

    return result;
}

void CreateGlTexture(GLuint texture, GraphicsApi::TextureType type, GraphicsApi::TextureFilter filter,
                     GraphicsApi::TextureMipmap mimpamp, const vec2ui& size, const void* data)
{
    auto params = GetTextureTypeParams(type);

    glBindTexture(params.target, texture);
    glTexImage2D(params.target, 0, params.internalFormat, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0,
                 params.format, params.dataType, data);
    glTexParameterf(params.target, GL_TEXTURE_MIN_FILTER, textureFilterMap_.at(filter));
    glTexParameterf(params.target, GL_TEXTURE_MAG_FILTER, textureFilterMap_.at(filter));

    if (mimpamp == GraphicsApi::TextureMipmap::LINEAR)
    {
        glGenerateMipmap(params.target);
        glTexParameteri(params.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(params.target, GL_TEXTURE_LOD_BIAS, 0);
    }

    glTexParameteri(params.target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(params.target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(params.target, 0);
}

GraphicsApi::ID OpenGLApi::CreateTexture(const Utils::Image& image, GraphicsApi::TextureFilter filter,
                                         GraphicsApi::TextureMipmap mipmap)
{
    GLuint texture;
    glGenTextures(1, &texture);

    auto errorString = GetGlError();
    if (not errorString.empty())
    {
        ERROR_LOG(errorString);
        return {};
    }
    GraphicsApi::TextureType type{GraphicsApi::TextureType ::U8_RGBA};
    uint32 dataTypeSize = 0;
    auto channels       = image.getChannelsCount();
    std::visit(
        visitor{
            [&](const std::vector<uint8>&)
            {
                switch (channels)
                {
                    case 4:
                        type         = GraphicsApi::TextureType::U8_RGBA;
                        dataTypeSize = sizeof(uint8) * 4;
                        break;
                    default:
                        DEBUG_LOG("Not implmented.");
                }
            },
            [&](const std::vector<float>&)
            {
                switch (channels)
                {
                    case 1:
                        type         = GraphicsApi::TextureType::FLOAT_TEXTURE_1D;
                        dataTypeSize = sizeof(float);
                        break;
                    case 2:
                        type         = GraphicsApi::TextureType::FLOAT_TEXTURE_2D;
                        dataTypeSize = sizeof(float) * 2;
                        break;
                    case 3:
                        type         = GraphicsApi::TextureType::FLOAT_TEXTURE_3D;
                        dataTypeSize = sizeof(float) * 3;
                        break;
                    case 4:
                        type         = GraphicsApi::TextureType::FLOAT_TEXTURE_4D;
                        dataTypeSize = sizeof(float) * 4;
                        break;
                    default:
                        DEBUG_LOG("Not implmented.");
                }
                dataTypeSize = sizeof(float);
            },
            [](std::monostate) { ERROR_LOG("Image data not set!"); },
        },
        image.getImageData());

    CreateGlTexture(texture, type, filter, mipmap, image.size(), image.getRawDataPtr());

    int64 bytes = image.size().x * image.size().y * dataTypeSize;
    allocatedBytes(bytes);

    auto rid = impl_->idPool_.ToUint(texture);
    createdObjectIds.insert({rid, ObjectType::TEXTURE_2D});
    allocatedTextureBytes.insert({rid, bytes});

    GraphicsApi::TextureInfo texutreInfo;
    texutreInfo.id            = rid;
    texutreInfo.size          = image.size();
    texutreInfo.textureFilter = filter;
    texutreInfo.textureMipmap = mipmap;
    texutreInfo.textureType   = type;

    impl_->textureInfos_.insert({rid, texutreInfo});

    return rid;
}

GraphicsApi::ID OpenGLApi::CreateTextureStorage(GraphicsApi::TextureType, GraphicsApi::TextureFilter filter, int32 N)
{
    GLuint texture;
    glGenTextures(1, &texture);

    auto errorString = GetGlError();
    if (not errorString.empty())
    {
        ERROR_LOG(errorString);
        return {};
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, static_cast<GLsizei>(log(N) / log(2)), GL_RGBA32F, N, N);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilterMap_[filter]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilterMap_[filter]);

    auto rid = impl_->idPool_.ToUint(texture);
    createdObjectIds.insert({rid, ObjectType::TEXTURE_2D});

    GraphicsApi::TextureInfo texutreInfo;
    texutreInfo.id            = rid;
    texutreInfo.size          = vec2ui(static_cast<uint32>(N));
    texutreInfo.textureFilter = filter;
    texutreInfo.textureMipmap = GraphicsApi::TextureMipmap::NONE;
    texutreInfo.textureType   = GraphicsApi::TextureType::FLOAT_TEXTURE_4D;
    impl_->textureInfos_.insert({rid, texutreInfo});

    return rid;
}

GraphicsApi::ID OpenGLApi::CreateCubMapTexture(const std::array<Utils::Image, 6>& images)
{
    uint32 id;
    glGenTextures(1, &id);
    GLenum hubo_error = glGetError();

    if (hubo_error)
    {
        return {};
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    int i = 0;
    int64 bytes{0};

    for (const auto& image : images)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i++), 0, GL_RGBA,
                     static_cast<GLsizei>(image.width), static_cast<GLsizei>(image.height), 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, image.getRawDataPtr());
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        bytes += image.width * image.height * 4;
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    allocatedBytes(bytes);

    auto rid              = impl_->idPool_.ToUint(id);
    createdObjectIds[rid] = ObjectType::TEXTURE_CUBE_MAP;
    allocatedTextureBytes.insert({rid, bytes});
    return rid;
}

void OpenGLApi::UpdateTexture(uint32 id, const vec2ui& offset, const Utils::Image& image)
{
    if (image.empty())
    {
        ERROR_LOG("Update image without data");
        return;
    }
    auto iter = impl_->textureInfos_.find(id);
    if (iter == impl_->textureInfos_.end())
    {
        return;
    }

    auto& textureInfo = iter->second;
    auto params       = GetTextureTypeParams(textureInfo.textureType);

    BindTexture(id);
    glTexSubImage2D(params.target, 0, static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                    static_cast<GLsizei>(image.width), static_cast<GLsizei>(image.height), params.format,
                    params.dataType, image.getRawDataPtr());
}
void OpenGLApi::UpdateTexture(uint32 id, const Utils::Image& image)
{
    auto iter = impl_->textureInfos_.find(id);
    if (iter == impl_->textureInfos_.end())
    {
        ERROR_LOG("Texture not found id : " + std::to_string(id));
        return;
    }

    auto& textureInfo = iter->second;
    auto glId         = impl_->idPool_.ToGL(id);

    CreateGlTexture(glId, textureInfo.textureType, textureInfo.textureFilter, textureInfo.textureMipmap, image.size(),
                    image.getRawDataPtr());
    textureInfo.size = image.size();
}

void OpenGLApi::ClearTexture(uint32 id, const Color& color)
{
    glClearTexImage(impl_->idPool_.ToGL(id), 0, GL_RGBA, GL_FLOAT, &color.value);
}

void OpenGLApi::EnableBlend()
{
    glEnable(GL_BLEND);
}

void OpenGLApi::DisableBlend()
{
    glDisable(GL_BLEND);
}

void OpenGLApi::EnableDepthMask()
{
    glDepthMask(GL_TRUE);
}

void OpenGLApi::DisableDepthMask()
{
    glDepthMask(GL_FALSE);
}

void OpenGLApi::ActiveTexture(uint32 nr)
{
    glActiveTexture(GL_TEXTURE0 + nr);
}

void OpenGLApi::ActiveTexture(uint32 nr, uint32 id)
{
    // if (id == 0 or not createdObjectIds.count(id))
    //{
    //    ERROR_LOG("Wrong image id : " + std::to_string(id));
    //    return;
    //}

    auto openGLId = impl_->idPool_.ToGL(id);

    glActiveTexture(GL_TEXTURE0 + nr);

    if (createdObjectIds.count(id) and createdObjectIds.at(id) == ObjectType::TEXTURE_CUBE_MAP)
        glBindTexture(GL_TEXTURE_CUBE_MAP, openGLId);
    else
        glBindTexture(GL_TEXTURE_2D, openGLId);
}

void OpenGLApi::DeleteObject(uint32 id)
{
    if (id == 0)
    {
        return;
    }

    if (createdObjectIds.count(id) == 0)
    {
        ERROR_LOG("Delete object error. Object with id : " + std::to_string(id) + " not created?");
        return;
    }

    auto openGLId = impl_->idPool_.ToGL(id);

    switch (createdObjectIds.at(id))
    {
        case ObjectType::SHADER_PROGRAM:
            DeleteShader(id);
            break;
        case ObjectType::TEXTURE_2D:
        {
            glDeleteTextures(1, &openGLId);
            auto iter = allocatedTextureBytes.find(id);
            if (iter != allocatedTextureBytes.end())
                allocatedBytes(-1 * iter->second);
            break;
        }
        case ObjectType::TEXTURE_CUBE_MAP:
        {
            glDeleteTextures(1, &openGLId);

            auto iter = allocatedTextureBytes.find(id);
            if (iter != allocatedTextureBytes.end())
                allocatedBytes(-1 * iter->second);
            break;
        }
        case ObjectType::RENDER_BUFFER:
            glDeleteRenderbuffers(1, &openGLId);
            break;
            // case ObjectType::FRAME_BUFFER:
            // DeleteFrameBuffer(id);
            break;
        case ObjectType::MESH:
            DeleteMesh(id);
            break;

        default:
            break;
    }

    // DEBUG_LOG("Delete object :" + std::to_string(id));
    createdObjectIds.erase(id);
}

void OpenGLApi::DeleteShaderBuffer(uint32 id)
{
    auto& bufferId   = impl_->shaderBuffers_[id];
    bufferId.isInGpu = false;
    glDeleteBuffers(1, &bufferId.glId);
}

std::string OpenGLApi::GetBufferStatus()
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        ERROR_LOG("FB error, status: " + std::to_string(status));
        return std::to_string(status);
    }

    return std::string();
}

GraphicsApi::ID OpenGLApi::CreatePatchMesh(const std::vector<float>& patches)
{
    if (not impl_->maxPatchVertices_)
    {
        return {};
    }

    auto rid = impl_->idPool_.ToUint(0);
    createdObjectIds.insert({rid, ObjectType::MESH});
    openGlMeshes_.insert({rid, {}});

    auto& mesh = openGlMeshes_.at(rid);
    VaoCreator vaoCreator;
    vaoCreator.AddStaticAttributePatch(VertexBufferObjects::POSITION, 2, patches);
    mesh             = Convert(vaoCreator.Get());
    mesh.renderType  = GraphicsApi::RenderType::PATCHES;
    mesh.vertexCount = static_cast<GLsizei>(patches.size());
    return rid;
}

GraphicsApi::ID OpenGLApi::CreatePurePatchMeshInstanced(uint32 patch, uint32 count)
{
    if (not impl_->maxPatchVertices_)
    {
        return {};
    }

    uint32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glPatchParameteri(GL_PATCH_VERTICES, patch);

    auto rid = impl_->idPool_.ToUint(vao);
    createdObjectIds.insert({rid, ObjectType::MESH});
    openGlMeshes_.insert({rid, {}});
    openGlMeshes_.at(rid).instancesCount = count;
    openGlMeshes_.at(rid).patches        = patch;
    openGlMeshes_.at(rid).renderType     = GraphicsApi::RenderType::PATCHES;
    return rid;
}

GraphicsApi::ID OpenGLApi::CreateMesh(const GraphicsApi::MeshRawData& meshRawData, GraphicsApi::RenderType type)
{
    auto rid = impl_->idPool_.ToUint(0);
    createdObjectIds.insert({rid, ObjectType::MESH});

    openGlMeshes_.insert({rid, {}});
    auto& mesh = openGlMeshes_.at(rid);

    CreateDebugNormalMesh(rid, meshRawData);

    VaoCreator vaoCreator;
    vaoCreator.AddIndicesBuffer(meshRawData.indices_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::POSITION, 3, meshRawData.positions_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TEXT_COORD, 2, meshRawData.textCoords_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::NORMAL, 3, meshRawData.normals_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TANGENT, 3, meshRawData.tangents_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::WEIGHTS, GraphicsApi::MAX_BONES_PER_VERTEX,
                                  meshRawData.bonesWeights_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::JOINTS, GraphicsApi::MAX_BONES_PER_VERTEX, meshRawData.joinIds_);
    mesh            = Convert(vaoCreator.Get());
    mesh.renderType = type;

    allocatedBytes(vaoCreator.Get().sizeInBytes);
    return rid;
}

GraphicsApi::ID OpenGLApi::CreateDynamicLineMesh()
{
    auto rid = impl_->idPool_.ToUint(0);
    createdObjectIds.insert({rid, ObjectType::MESH});

    openGlMeshes_.insert({rid, {}});
    auto& mesh = openGlMeshes_.at(rid);

    VaoCreator vaoCreator;
    vaoCreator.SetSize(0);
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::POSITION, 3);
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::NORMAL, 3);

    allocatedBytes(vaoCreator.Get().sizeInBytes);

    mesh            = Convert(vaoCreator.Get());
    mesh.renderType = GraphicsApi::RenderType::LINES;
    return rid;
}

GraphicsApi::ID OpenGLApi::CreateParticle()
{
    auto rid = impl_->idPool_.ToUint(0);
    createdObjectIds.insert({rid, ObjectType::MESH});

    openGlMeshes_.insert({rid, {}});
    auto& mesh = openGlMeshes_.at(rid);

    FloatAttributeVec vertex      = {-1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0};
    FloatAttributeVec text_coords = {0, 0, 0, 1, 1, 1, 1, 0};
    IndicesVector indices         = {0, 1, 3, 3, 1, 2};

    VaoCreator vaoCreator;
    vaoCreator.AddIndicesBuffer(indices);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::POSITION, 3, vertex);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TEXT_COORD, 2, text_coords);
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::TRANSFORM_MATRIX, 4, sizeof(mat4));
    mesh = Convert(vaoCreator.Get());
    allocatedBytes(vaoCreator.Get().sizeInBytes);
    return rid;
}
GraphicsApi::ID OpenGLApi::CreateAnimatedParticle()
{
    auto rid = impl_->idPool_.ToUint(0);
    createdObjectIds.insert({rid, ObjectType::MESH});

    openGlMeshes_.insert({rid, {}});
    auto& mesh = openGlMeshes_.at(rid);

    FloatAttributeVec vertex      = {-1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0};
    FloatAttributeVec text_coords = {0, 0, 0, 1, 1, 1, 1, 0};
    IndicesVector indices         = {0, 1, 3, 3, 1, 2};

    VaoCreator vaoCreator;
    vaoCreator.AddIndicesBuffer(indices);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::POSITION, 3, vertex);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TEXT_COORD, 2, text_coords);
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::TRANSFORM_MATRIX, 4, sizeof(mat4));
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::TEXTURE_OFFSET, 4, sizeof(vec4));
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::BLEND_FACTOR, 1, sizeof(float));
    mesh = Convert(vaoCreator.Get());

    allocatedBytes(vaoCreator.Get().sizeInBytes);
    return rid;
}
void OpenGLApi::Compute(uint32 x, uint32 y, uint32 z)
{
    glDispatchCompute(x, y, z);
    glFinish();
}
struct FloatBufferMatrix
{
    std::vector<float> rows[4];
};

void pushRow(const mat4& matrix, int row, std::vector<float>& dest)
{
    dest.push_back(matrix[row][0]);
    dest.push_back(matrix[row][1]);
    dest.push_back(matrix[row][2]);
    dest.push_back(matrix[row][3]);
}

FloatBufferMatrix GetFloatBufferMatrix(const std::vector<mat4>& matrixes)
{
    FloatBufferMatrix result;
    result.rows[0].reserve(matrixes.size() * 4);
    result.rows[1].reserve(matrixes.size() * 4);
    result.rows[2].reserve(matrixes.size() * 4);
    result.rows[3].reserve(matrixes.size() * 4);
    for (const auto& mat : matrixes)
    {
        for (uint8 i = 0; i < 4; ++i)
        {
            pushRow(mat, i, result.rows[i]);
        }
    }
    return result;
}

void OpenGLApi::UpdateMatrixes(uint32 objectId, const std::vector<mat4>& mat)
{
    auto iter = openGlMeshes_.find(objectId);
    if (iter == openGlMeshes_.end())
        return;
    auto& obj = iter->second;

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * mat.size(), &mat[0], GL_STREAM_DRAW);
}

void UpdateVBO(OpenGLMesh& obj, VertexBufferObjects type, const std::vector<float>& data)
{
    glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[type]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], GL_STREAM_DRAW);
}

void OpenGLApi::UpdateMesh(uint32 objectId, const GraphicsApi::MeshRawData& data,
                           const std::set<VertexBufferObjects>& buffers)
{
    auto iter = openGlMeshes_.find(objectId);
    if (iter == openGlMeshes_.end())
        return;
    auto& obj = iter->second;

    for (auto& buffer : buffers)
    {
        switch (buffer)
        {
            case VertexBufferObjects::POSITION:
                if (not obj.useIndiecies)
                {
                    obj.vertexCount = static_cast<GLsizei>(data.positions_.size() / 3);
                }
                UpdateVBO(obj, buffer, data.positions_);
                break;
            case VertexBufferObjects::TEXT_COORD:
                UpdateVBO(obj, buffer, data.textCoords_);
                break;
            case VertexBufferObjects::NORMAL:
                UpdateVBO(obj, buffer, data.normals_);
                break;
            case VertexBufferObjects::TANGENT:
                UpdateVBO(obj, buffer, data.tangents_);
                break;
            default:
                DEBUG_LOG("Update not implemented.");
        }
    }

    CreateDebugNormalMesh(objectId, data);
}
void OpenGLApi::UpdateLineMesh(uint32 objectId, const GraphicsApi::LineMesh& mesh)
{
    auto iter = openGlMeshes_.find(objectId);
    if (iter == openGlMeshes_.end())
        return;

    auto& obj       = iter->second;
    obj.vertexCount = static_cast<GLsizei>(mesh.positions_.size());

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::POSITION]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.positions_.size(), &mesh.positions_[0], GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::NORMAL]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.colors_.size(), &mesh.colors_[0], GL_STREAM_DRAW);
}
void OpenGLApi::UpdateOffset(uint32 objectId, const std::vector<vec4>& offset)
{
    auto iter = openGlMeshes_.find(objectId);
    if (iter == openGlMeshes_.end())
        return;
    auto& mesh = iter->second;

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[VertexBufferObjects::TEXTURE_OFFSET]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * offset.size(), &offset[0], GL_STREAM_DRAW);
}
void OpenGLApi::UpdateBlend(uint32 objectId, const std::vector<float>& blendFactor)
{
    auto iter = openGlMeshes_.find(objectId);
    if (iter == openGlMeshes_.end())
        return;
    auto& mesh = iter->second;

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[VertexBufferObjects::BLEND_FACTOR]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * blendFactor.size(), &blendFactor[0], GL_STREAM_DRAW);
}
void OpenGLApi::RenderPurePatchedMeshInstances(uint32 id)
{
    auto iter = openGlMeshes_.find(id);
    if (iter == openGlMeshes_.end())
        return;
    auto& mesh = iter->second;

    glBindVertexArray(mesh.vao);
    glDrawArraysInstanced(GL_PATCHES, 0, mesh.patches, mesh.instancesCount);
}

void OpenGLApi::RenderMesh(uint32 id)
{
    auto iter = openGlMeshes_.find(id);
    if (iter == openGlMeshes_.end())
        return;

    auto& mesh = iter->second;

    glBindVertexArray(mesh.vao);

    if (mesh.useIndiecies)
    {
        glDrawElements(renderTypeMap_.at(mesh.renderType), mesh.vertexCount, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(renderTypeMap_.at(mesh.renderType), 0, mesh.vertexCount);
    }
}

void OpenGLApi::RenderTriangleStripMesh(uint32 id)
{
    auto iter = openGlMeshes_.find(id);
    if (iter == openGlMeshes_.end())
        return;

    auto& mesh = iter->second;

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLE_STRIP, mesh.vertexCount, GL_UNSIGNED_INT, nullptr);
}

void OpenGLApi::RenderMeshInstanced(uint32 id, uint32 istanced)
{
    auto iter = openGlMeshes_.find(id);
    if (iter == openGlMeshes_.end())
        return;

    auto& mesh = iter->second;
    glBindVertexArray(mesh.vao);

    if (mesh.useIndiecies)
    {
        glDrawElementsInstanced(renderTypeMap_.at(mesh.renderType), mesh.vertexCount, GL_UNSIGNED_INT, nullptr,
                                istanced);
    }
    else
    {
        glDrawArraysInstanced(renderTypeMap_.at(mesh.renderType), 0, mesh.vertexCount, istanced);
    }
}

void OpenGLApi::RenderDebugNormals(uint32 referentMeshId)
{
    auto meshIdIter = impl_->debugNormalsMesh_.find(referentMeshId);
    if (meshIdIter != impl_->debugNormalsMesh_.end())
    {
        if (meshIdIter->second.id_)
        {
            RenderMesh(*meshIdIter->second.id_);
        }
    }
}

void OpenGLApi::RenderPoints(uint32 id)
{
    auto iter = openGlMeshes_.find(id);
    if (iter == openGlMeshes_.end())
        return;
    auto& mesh = iter->second;
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_POINTS, 0, mesh.vertexCount);
}

void OpenGLApi::RenderQuad()
{
    quad_.Render(2);
}

void OpenGLApi::RenderQuadTs()
{
    quadTs_.Render(2);
}

void OpenGLApi::EnableClipingPlane(uint32 i)
{
    glEnable(GL_CLIP_DISTANCE0 + i);
}

void OpenGLApi::DisableCliping(uint32 i)
{
    glDisable(GL_CLIP_DISTANCE0 + i);
}

void OpenGLApi::EnableCulling()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void OpenGLApi::DisableCulling()
{
    glDisable(GL_CULL_FACE);
}

void OpenGLApi::SetViewPort(uint32 x, uint32 y, uint32 width, uint32 height)
{
    glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLint>(width), static_cast<GLint>(height));
}

void OpenGLApi::BindTexture(uint32 id) const
{
    glBindTexture(GL_TEXTURE_2D, impl_->idPool_.ToGL(id));
}

void OpenGLApi::BindImageTexture(uint32 id, GraphicsApi::TextureAccess acces)
{
    auto iter = textureAccessMap_.find(acces);
    if (iter != textureAccessMap_.end())
    {
        glActiveTexture(0);
        glBindImageTexture(0, impl_->idPool_.ToGL(id), 0, false, 0, iter->second, GL_RGBA32F);
    }
}

GraphicsApi::ID OpenGLApi::CreateShadowMap(uint32 sizex, uint32 sizey)
{
    auto glId = CreateDepthBufferAttachment(sizex, sizey);
    auto rid  = impl_->idPool_.ToUint(glId);
    createdObjectIds.insert({rid, ObjectType::TEXTURE_2D});

    allocatedBytes(sizex * sizey * sizeof(float));
    return rid;
}

void OpenGLApi::PolygonModeRender()
{
    if (polygonMode_)
        return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    polygonMode_ = true;
}

void OpenGLApi::LineModeRender()
{
    if (not polygonMode_)
        return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    polygonMode_ = false;
}

void OpenGLApi::SetBlendFunction(GraphicsApi::BlendFunctionType type)
{
    glBlendEquation(GL_FUNC_ADD);
    switch (type)
    {
        case GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case GraphicsApi::BlendFunctionType::ONE:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case GraphicsApi::BlendFunctionType::ZERO:
            glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
            break;
        case GraphicsApi::BlendFunctionType::SRC_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
            break;
        case GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
            break;
        case GraphicsApi::BlendFunctionType::DST_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
            break;
        case GraphicsApi::BlendFunctionType::SRC_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
            break;
        case GraphicsApi::BlendFunctionType::ONE_MINUS_DST_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_COLOR);
            break;
    }
}
void OpenGLApi::allocatedBytes(int64 bytes)
{
    allocatedBytes_ += bytes;
    // DEBUG_LOG("Textures + meshes, allocatedBytes = " + std::to_string(allocatedBytes_) + " (" +
    //         std::to_string(allocatedBytes_ / 1024 / 1024) + "MB)");
}
}  // namespace OpenGLApi
