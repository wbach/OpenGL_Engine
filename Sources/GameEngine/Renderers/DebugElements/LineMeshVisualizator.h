#pragma once
#include <atomic>

#include "GameEngine/Shaders/ShaderProgram.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace Utils
{
namespace Thread
{
class Worker;
class IThreadSync;
}  // namespace Thread
}  // namespace Utils

namespace GameEngine
{
class LineMeshVisualizator
{
public:
    LineMeshVisualizator(GraphicsApi::IGraphicsApi&, Utils::Thread::IThreadSync&);
    ~LineMeshVisualizator();
    void Init();
    void Render();
    void SetMeshCreationFunction(std::function<const GraphicsApi::LineMesh&()>);
    void ReloadShader();

private:
    void UpdateLineMesh();
    void UpdateByWorker();
    bool IsReady() const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    Utils::Thread::IThreadSync& threadSync_;
    ShaderProgram shader_;
    GraphicsApi::ID defaultPerObjectUpdateId_;

    std::function<const GraphicsApi::LineMesh&()> createLineMesh;

    uint32 refreshRateStepDown_;
    GraphicsApi::ID lineMeshId_;
    std::atomic_bool lineMeshReady_;

    uint32 frameRefreshNumber_;
    Utils::Thread::Worker* worker_;
    std::atomic_bool isUpdated_;
    const GraphicsApi::LineMesh* lineMesh_;
    bool useWorkerToUpdate_;
};
}  // namespace GameEngine
