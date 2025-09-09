#include "AnimationViewerFrame.h"

#include <GameEngine/Animations/AnimationUtils.h>
#include <GameEngine/Camera/ThridPersonCamera.h>
#include <GameEngine/Components/Animation/Animator.h>
#include <GameEngine/Engine/Engine.h>
#include <GameEngine/Physics/Bullet/BulletAdapter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <wx/dnd.h>

#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <GameEngine/Scene/Scene.hpp>
#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <cmath>
#include <filesystem>
#include <memory>
#include <string>

#include "Components/FunctionType.h"
#include "Input/KeyCodes.h"
#include "Logger/Log.h"
#include "ProjectManager.h"

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

    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);

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

    wxPanel* rightPanel    = new wxPanel(mainSplitter);
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);

    // Label lista animacji
    wxStaticText* label = new wxStaticText(rightPanel, wxID_ANY, "Lista animacji:");
    rightSizer->Add(label, 0, wxALL, 5);

    animList = new wxListBox(rightPanel, wxID_ANY);
    rightSizer->Add(animList, 1, wxEXPAND | wxALL, 5);

    animList->Bind(wxEVT_CONTEXT_MENU, &AnimationViewerFrame::OnAnimListContextMenu, this);
    animList->Bind(wxEVT_LISTBOX,
                   [this](wxCommandEvent&)
                   {
                       if (not currentGameObject)
                           return;

                       wxString selected = animList->GetStringSelection();
                       if (!selected.IsEmpty())
                       {
                           currentGameObject->animator.ChangeAnimation(
                               selected.ToStdString(), GameEngine::Components::Animator::AnimationChangeType::smooth);
                           LOG_DEBUG << "Changed animation to: " << selected.ToStdString();
                       }
                   });

    wxBoxSizer* folderRowSizer = new wxBoxSizer(wxHORIZONTAL);

    animationsSearchFolderPath = new wxStaticText(rightPanel, wxID_ANY, "-");
    folderRowSizer->Add(animationsSearchFolderPath, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    wxButton* chooseFolderBtn = new wxButton(rightPanel, wxID_ANY, "Read animations from directory");
    folderRowSizer->Add(chooseFolderBtn, 0, wxALL, 5);

    rightSizer->Add(folderRowSizer, 0, wxEXPAND);

    // Obsługa kliknięcia przycisku
    chooseFolderBtn->Bind(wxEVT_BUTTON,
                          [this](wxCommandEvent&)
                          {
                              if (not currentGameObject)
                                  return;

                              auto startupPath = currentGameObject->currentModelFile.has_value()
                                                     ? currentGameObject->currentModelFile->GetParentDir()
                                                     : ProjectManager::GetInstance().GetProjectPath();
                              wxDirDialog dlg(this, "Read animations from directory", startupPath, wxDD_DIR_MUST_EXIST);
                              if (dlg.ShowModal() == wxID_OK)
                              {
                                  wxString path = dlg.GetPath();

                                  FindAllAnimationsInFolder(currentGameObject->animator, path.ToStdString());

                                  auto modelName =
                                      currentGameObject->rendererComponent.GetModelWrapper().Get()->GetFile().GetBaseName();
                                  SetStatusText(modelName + ". Path: " + path);
                              }
                          });

    rightPanel->SetSizer(rightSizer);

    auto size = GetSize();
    mainSplitter->SplitVertically(canvas, rightPanel, 3 * size.x / 4);

    CreateStatusBar();
    SetStatusText("Animation Viewer ready");

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

    if (currentGameObject)
    {
        if (not currentGameObject->gameObjectId)
        {
            LOG_ERROR << "Something goes wrong, return";
            return;
        }
        scene.RemoveGameObject(currentGameObject->gameObjectId.value());
        currentGameObject.reset();
    }

    auto newGameObject      = scene.CreateGameObject(modelFile.GetBaseName());
    auto& rendererComponent = newGameObject->AddComponent<GameEngine::Components::RendererComponent>();
    auto& animator          = newGameObject->AddComponent<GameEngine::Components::Animator>();

    animator.startupAnimationClipName = "noname";
    rendererComponent.AddModel(modelFile.GetAbsoultePath());
    animationsSearchFolderPath->SetLabel(modelFile.GetBaseName());

    LOG_DEBUG << "Object created. Id = " << newGameObject->GetId();

    if (not scene.getInputManager())
    {
        LOG_ERROR << "not scene.getInputManager()";
        return;
    }

    currentGameObject.emplace(CurrentGameObject{.gameObjectId      = newGameObject->GetId(),
                                                .currentModelFile  = modelFile,
                                                .animator          = animator,
                                                .rendererComponent = rendererComponent});

    scene.AddGameObject(std::move(newGameObject));

    const auto& animationClips = animator.getAnimationClips();
    for (const auto& [name, _] : animationClips)
    {
        animList->Append(name);
    }

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
    if (currentGameObject)
    {
        auto& scene = canvas->GetScene();
        scene.getComponentController().CallGameObjectFunctions(GameEngine::Components::FunctionType::Update,
                                                               currentGameObject->gameObjectId.value());
    }
}
void AnimationViewerFrame::FindAllAnimationsInFolder(GameEngine::Components::Animator& animator, const std::string& path)
{
    if (not animList->IsEmpty())
    {
        int answer = wxMessageBox("List not empty, clear current anims?", "Confirmation", wxYES_NO | wxICON_QUESTION);

        if (answer == wxYES)
        {
            animList->Clear();
            animator.clearAnimationClips();
        }
        else
        {
            int answer = wxMessageBox("Continue anyway?", "Confirmation", wxYES_NO | wxICON_QUESTION);

            if (answer != wxYES)
            {
                return;
            }
        }
    }
    auto files = Utils::FindFilesWithExtension(path, ".xml");
    if (not files.empty())
    {
        for (const auto& file : files)
        {
            DEBUG_LOG("Found animation file in subfolders add clip : " + file);
            if (auto animationName = GameEngine::Animation::IsAnimationClip(file))
            {
                animator.AddAnimationClip(*animationName, GameEngine::File(file));
                animList->Append(*animationName);
            }
        }

        auto animationName = GameEngine::Animation::IsAnimationClip(files.front());
        if (animationName)
        {
            animator.startupAnimationClipName = *animationName;
        }

        animator.initAnimationClips();
    }
}

void AnimationViewerFrame::OnAnimListContextMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
    menu.Append(10001, "Export to file");

    Bind(wxEVT_MENU, &AnimationViewerFrame::OnExportToFile, this, 10001);

    PopupMenu(&menu);
}

void AnimationViewerFrame::OnExportToFile(wxCommandEvent& event)
{
    if (not currentGameObject)
        return;

    wxString selected = animList->GetStringSelection();
    if (selected.IsEmpty())
    {
        wxMessageBox("No animation selected!", "Export", wxOK | wxICON_WARNING);
        return;
    }

    wxFileDialog saveFileDialog(this, "Save animation to file", currentGameObject->currentModelFile->GetParentDir(),
                                selected + ".xml", "Animation files (*.xml)|*.xml|All files (*.*)|*.*",
                                wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString path = saveFileDialog.GetPath();

    LOG_DEBUG << "Exporting animation " << selected.ToStdString() << " to file: " << path.ToStdString();

    auto model = currentGameObject->rendererComponent.GetModelWrapper().Get();
    if (model)
    {
        auto maybeRootJoint = model->getRootJoint();
        const auto& clips   = currentGameObject->animator.getAnimationClips();
        auto iter           = clips.find(selected.ToStdString());

        if (maybeRootJoint and iter != clips.end())
        {
            GameEngine::Animation::ExportAnimationClipToFile(path.ToStdString(), iter->second.clip, *maybeRootJoint,
                                                             wxFileName(saveFileDialog.GetFilename()).GetName().ToStdString());
        }
    }

    wxMessageBox("Exported " + selected + " to " + path, "Export", wxOK | wxICON_INFORMATION);
}