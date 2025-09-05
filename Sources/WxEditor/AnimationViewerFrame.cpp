#include "AnimationViewerFrame.h"

#include <GameEngine/Camera/ThridPersonCamera.h>
#include <GameEngine/Components/Animation/Animator.h>
#include <GameEngine/Engine/Engine.h>
#include <GameEngine/Physics/Bullet/BulletAdapter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <wx/dnd.h>

#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <GameEngine/Scene/Scene.hpp>
#include <cmath>
#include <memory>

#include "Components/FunctionType.h"
#include "Input/KeyCodes.h"
#include "Logger/Log.h"

namespace
{
class FileDropTarget : public wxFileDropTarget
{
public:
    FileDropTarget(AnimationViewerFrame* panel)
        : viewer(panel)
    {
    }

    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override
    {
        LOG_DEBUG << "OnDropFiles";

        if (!viewer)
            return false;

        if (filenames.empty())
            return false;

        if (filenames.size() > 1)
        {
            LOG_DEBUG << "Only one file is supported";
        }

        LOG_DEBUG << "OnDropFiles: " << filenames[0];

        viewer->ShowModel(filenames[0].ToStdString());

        return true;
    }

private:
    AnimationViewerFrame* viewer;
};
}  // namespace

AnimationViewerFrame::AnimationViewerFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    Init();
}

void AnimationViewerFrame::Init()
{
    wxInitAllImageHandlers();

    // === Główny splitter: lewo/prawo ===
    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);

    // --- Canvas po lewej ---
    auto onStartupDone = [this]()
    {
        auto& camera = canvas->GetScene().GetCamera();
        camera.SetPosition(vec3(-0.75, 0.5, 0.75));
        camera.LookAt(vec3(0, 0.5, 0));
        camera.UpdateMatrix();
        isInit = true;
        if (showModelAfterInit)
        {
            ShowModel(*showModelAfterInit);
        }
    };
    auto selectItemInGameObjectTree = [](uint32, bool) {};

    canvas = new GLCanvas(mainSplitter, onStartupDone, selectItemInGameObjectTree);
    canvas->SetDropTarget(new FileDropTarget(this));

    // --- Panel po prawej (lista animacji + potencjalnie inne widgety) ---
    wxPanel* rightPanel    = new wxPanel(mainSplitter);
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(rightPanel, wxID_ANY, "Lista animacji:");
    rightSizer->Add(label, 0, wxALL, 5);

    animList = new wxListBox(rightPanel, wxID_ANY);
    rightSizer->Add(animList, 1, wxEXPAND | wxALL, 5);

    rightPanel->SetSizer(rightSizer);

    // --- Podział na lewo/prawo ---
    auto size = GetSize();
    mainSplitter->SplitVertically(canvas, rightPanel, 3 * size.x / 4);

    // === Status bar / menu (opcjonalnie) ===
    CreateStatusBar();
    SetStatusText("Animation Viewer ready");

    // przykładowe animacje
    animList->Append("Idle");
    animList->Append("Walk");
    animList->Append("Run");
    animList->Append("Jump");

    timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &AnimationViewerFrame::OnTimer, this);
    timer->Start(16);
}

void AnimationViewerFrame::ShowModel(const GameEngine::File& modelFile)
{
    if (not isInit)
    {
        showModelAfterInit = modelFile;
        return;
    }

    if (not CheckExtension(modelFile))
    {
        LOG_DEBUG << "Externsion not supporeted.";
        return;
    }
    auto& scene = canvas->GetScene();

    if (gameObjectId)
    {
        scene.RemoveGameObject(*gameObjectId);
        gameObjectId.reset();
    }

    auto newGameObject      = scene.CreateGameObject(modelFile.GetBaseName());
    auto& rendererComponent = newGameObject->AddComponent<GameEngine::Components::RendererComponent>();
    auto& animator          = newGameObject->AddComponent<GameEngine::Components::Animator>();

    animator.startupAnimationClipName = "noname";
    rendererComponent.AddModel(modelFile.GetAbsoultePath());
    gameObjectId = newGameObject->GetId();

    LOG_DEBUG << "Object created. Id = " << gameObjectId;

    if (not scene.getInputManager())
    {
        LOG_ERROR << "not scene.getInputManager()";
        return;
    }

    scene.AddGameObject(std::move(newGameObject));

    //  scene.getComponentController().CallGameObjectFunctions(GameEngine::Components::FunctionType::OnStart, *gameObjectId);
}

bool AnimationViewerFrame::CheckExtension(const GameEngine::File& file)
{
    return file.IsExtension({"AMF", "3DS",      "AC",      "ASE", "ASSBIN", "B3D",  "BVH",   "COLLADA", "DXF", "CSM",
                             "DAE", "HMP",      "IRRMESH", "IRR", "LWO",    "LWS",  "MD2",   "MD3",     "MD5", "MD5MESH",
                             "MDC", "MDL",      "NFF",     "NDO", "OFF",    "OBJ",  "OGRE",  "OPENGEX", "PLY", "MS3D",
                             "COB", "BLEND",    "IFC",     "XGL", "FBX",    "Q3D",  "Q3BSP", "RAW",     "SIB", "SMD",
                             "STL", "TERRAGEN", "3D",      "X",   "X3D",    "GLTF", "3MF",   "MMD",     "STEP"});

    // AMF 3DS AC ASE ASSBIN B3D BVH COLLADA DXF CSM HMP IRRMESH IRR LWO LWS MD2 MD3 MD5 MDC MDL NFF NDO OFF OBJ
    // OGRE OPENGEX PLY MS3D COB BLEND IFC XGL FBX Q3D Q3BSP RAW SIB SMD STL TERRAGEN 3D X X3D GLTF 3MF MMD STEP
}
void AnimationViewerFrame::OnTimer(wxTimerEvent& event)
{
    if (gameObjectId)
    {
        auto& scene = canvas->GetScene();
        scene.getComponentController().CallGameObjectFunctions(GameEngine::Components::FunctionType::Update, *gameObjectId);
    }
}
