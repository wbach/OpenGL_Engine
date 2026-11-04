#ifdef __EMSCRIPTEN__
#include <Logger/Log.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>
#include <emscripten.h>

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Scene/SceneFactoryBase.h"
#include "GameEngine/Scene/SceneUtils.h"
#include "TreeNode.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace
{
const std::string CSTR_ROOT_NODE{"projectConfiguration"};
const std::string CSTR_STARTUP_SCENE_NODE{"startupScene"};
const std::string CSTR_SCENES_NODE{"scenes"};

std::unique_ptr<Engine> engine;

void EngineMainLoop(void* arg)
{
    auto* gameEngine = static_cast<Engine*>(arg);

    if (gameEngine)
    {
        gameEngine->MainLoop();
    }
}
}  // namespace

// clang-format off
EM_JS(void, start_webgpu, (), {
    (async () => {
        try {
            const canvas = document.querySelector("canvas");
            const adapter = await navigator.gpu.requestAdapter();

            if (!adapter) {
                throw new Error("Nie znaleziono adaptera GPU");
            }

            const device = await adapter.requestDevice();
            const context = canvas.getContext("webgpu");
            const format = navigator.gpu.getPreferredCanvasFormat();

            context.configure({
                device: device,
                format: format,
                alphaMode: "opaque"
            });

            const gpuShader = device.createShaderModule({
                code: `
                    @vertex
                    fn vs(@builtin(vertex_index) i: u32)
                        -> @builtin(position) vec4f {
                        var positions = array<vec2f, 3>(
                            vec2f( 0.0,  0.7),
                            vec2f(-0.7, -0.7),
                            vec2f( 0.7, -0.7)
                        );

                        return vec4f(positions[i], 0.0, 1.0);
                    }

                    @fragment
                    fn fs() -> @location(0) vec4f {
                        return vec4f(1.0, 0.2, 0.1, 1.0);
                    }
                `
            });

            const pipeline = device.createRenderPipeline({
                layout: "auto",
                vertex: {
                    ["module"]: gpuShader,
                    entryPoint: "vs"
                },
                fragment: {
                    ["module"]: gpuShader,
                    entryPoint: "fs",
                    targets: [{ format: format }]
                },
                primitive: {
                    topology: "triangle-list"
                }
            });

            function render() {
                const texture = context.getCurrentTexture();
                const encoder = device.createCommandEncoder();

                const pass = encoder.beginRenderPass({
                    colorAttachments: [{
                        view: texture.createView(),
                        clearValue: { r: 0.05, g: 0.05, b: 0.08, a: 1 },
                        loadOp: "clear",
                        storeOp: "store"
                    }]
                });

                pass.setPipeline(pipeline);
                pass.draw(3);
                pass.end();

                device.queue.submit([encoder.finish()]);
                requestAnimationFrame(render);
            }

            console.log("WebGPU: uruchamiam trójkąt!");
            render();
        } catch (error) {
            console.error("Błąd WebGPU:", error);
        }
    })();
});
// clang-format off


int main(int argc, char* argv[])
{
    std::puts("Aplikacja WebGPU uruchomiona.");

    CLogger::Instance().EnableLogs();
    CLogger::Instance().UseAsyncLogging(false);

    Utils::JsonReader jsonReader;

    const std::string inputFile = "/scenes.json";

    if (!jsonReader.Read(inputFile))
    {
        LOG_DEBUG << inputFile << " read failed";

        if (std::filesystem::exists(inputFile))
            return 1;

        GameEngine::createScenesFile(inputFile);
        return 1;
    }

    auto scenesNode = jsonReader.Get(CSTR_SCENES_NODE);

    PrintTree(*jsonReader.Get());

    if (!scenesNode)
    {
        LOG_ERROR << CSTR_SCENES_NODE
                    << " not found in project config file";

        return 1;
    }

    auto sceneFactory = std::make_unique<SceneFactoryBase>();

    for (const auto& sceneNode : scenesNode->getChildren())
    {
        if (!sceneNode)
            continue;

        LOG_DEBUG << "Add scene "
                    << sceneNode->name()
                    << ", "
                    << sceneNode->value_;

        sceneFactory->AddScene(
            sceneNode->name(),
            GameEngine::File{sceneNode->value_}
        );
    }

    if (sceneFactory->ScenesSize() < 1)
    {
        LOG_ERROR << "Scene not available";
        return 1;
    }

    engine = std::make_unique<Engine>(
        std::make_unique<Bullet::BulletAdapter>(),
        std::move(sceneFactory)
    );

    engine->Init();

    auto startupScene = jsonReader.Get(
        CSTR_STARTUP_SCENE_NODE
    );

    if (startupScene)
    {
        LOG_DEBUG << "Startup scene " << startupScene->value_;

        engine->GetSceneManager().SetActiveScene(
            startupScene->value_
        );
    }

    // Tymczasowy test WebGPU.
    start_webgpu();

    // Docelowa pętla silnika:
    emscripten_set_main_loop_arg(
        EngineMainLoop,
        engine.get(),
        0,
        0
    );

    return 0;

}
#endif