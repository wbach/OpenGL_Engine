#include "AnimationViewerFrame.h"

#include <GameEngine/Animations/AnimationUtils.h>
#include <GameEngine/Camera/ThridPersonCamera.h>
#include <GameEngine/Components/Animation/Animator.h>
#include <GameEngine/Engine/Engine.h>
#include <GameEngine/Physics/Bullet/BulletAdapter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <GameEngine/Scene/SceneReader.h>
#include <Input/KeyCodes.h>
#include <Logger/Log.h>
#include <Utils.h>
#include <wx/defs.h>
#include <wx/dnd.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <GameEngine/Scene/Scene.hpp>
#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <cmath>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>

#include "AnimationFileDropTarget.h"
#include "GameEngine/Components/FunctionType.h"
#include "WxEditor/EngineRelated/GLCanvas.h"
#include "WxEditor/ProjectManager.h"
#include "WxEditor/WxHelpers/EditorUitls.h"

AnimationViewerFrame::AnimationViewerFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    Init();
}

bool AnimationViewerFrame::isObjectSelcted() const
{
    return currentGameObject.has_value();
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

    canvas = new GLCanvas(mainSplitter, onStartupDone, selectItemInGameObjectTree, false);
    canvas->SetDropTarget(new AnimationFileDropTarget(this));

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

    rightPanel->SetSizer(rightSizer);

    auto size = GetSize();
    mainSplitter->SplitVertically(canvas, rightPanel, 3 * size.x / 4);

    CreateMainMenu();
    CreateStatusBar();
    SetStatusText("Animation Viewer ready");

    timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &AnimationViewerFrame::OnTimer, this);
    timer->Start(16);
}

void AnimationViewerFrame::CreateMainMenu()
{
    wxMenuBar* menuBar            = new wxMenuBar();
    int ID_OPEN_FILE              = wxWindow::NewControlId();
    int ID_READ_ANIMATIONS_FOLDER = wxWindow::NewControlId();
    int ID_EXPORT_SELECTED_CLIP   = wxWindow::NewControlId();
    int ID_EXPORT_ALL_CLIPS       = wxWindow::NewControlId();
    int ID_CREATE_PREFAB          = wxWindow::NewControlId();

    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_OPEN_FILE, "Open model");
    fileMenu->Append(ID_READ_ANIMATIONS_FOLDER, "Import animations from directory");
    fileMenu->Append(ID_EXPORT_SELECTED_CLIP, "Export slected clip");
    fileMenu->Append(ID_EXPORT_ALL_CLIPS, "Export all animation clips");
    fileMenu->Append(ID_CREATE_PREFAB, "Create prefab from current object");

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            wxFileDialog dlg(this, "Open mode", ProjectManager::GetInstance().GetProjectPath().string());
            if (dlg.ShowModal() == wxID_OK)
            {
                wxString path = dlg.GetPath();
                ShowModel(path.ToStdString());
            }
        },
        ID_OPEN_FILE);

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            if (not currentGameObject)
            {
                wxMessageBox("model not set, please drag or open model first", "Warning", wxOK | wxICON_WARNING);
                return;
            }

            wxDirDialog dlg(this, "Import animations from directory", GetStartupDialogPathBasedOnCurrent(), wxDD_DIR_MUST_EXIST);
            if (dlg.ShowModal() == wxID_OK)
            {
                wxString path = dlg.GetPath();
                SearchAndAddClipsFromDir(path.ToStdString());

                auto modelName = currentGameObject->rendererComponent.GetModelWrapper().Get()->GetFile().GetBaseName();
                SetStatusText(modelName + ". Path: " + path);
            }
        },
        ID_READ_ANIMATIONS_FOLDER);

    Bind(
        wxEVT_MENU, [this](wxCommandEvent& e) { OnExportToFile(e); }, ID_EXPORT_SELECTED_CLIP);

    Bind(
        wxEVT_MENU, [this](wxCommandEvent& e) { OnExportAll(e); }, ID_EXPORT_ALL_CLIPS);

    Bind(
        wxEVT_MENU, [this](wxCommandEvent& e) { CreatePrefab(e); }, ID_CREATE_PREFAB);

    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    menuBar->Append(fileMenu, "&File");

    Bind(
        wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    SetMenuBar(menuBar);
}

void AnimationViewerFrame::ShowModel(const GameEngine::File& file)
{
    if (not isInit)
    {
        showModelAfterInit = file;
        return;
    }

    if (is3dModelFile(file))
    {
        CreateGameObjectBasedOnModel(file);
        return;
    }

    if (isPrefab(file))
    {
        CreateGameObjectBasedOnPrefab(file);
        return;
    }

    LOG_DEBUG << "Externsion not supporeted.";
}

void AnimationViewerFrame::CreateGameObjectBasedOnModel(const GameEngine::File& modelFile)
{
    auto& scene = canvas->GetScene();

    if (currentGameObject)
    {
        if (not currentGameObject->gameObjectId)
        {
            LOG_ERROR << "Something goes wrong, return";
            return;
        }
        Clear();
    }

    auto newGameObject      = scene.CreateGameObject(modelFile.GetBaseName());
    auto& rendererComponent = newGameObject->AddComponent<GameEngine::Components::RendererComponent>();
    auto& animator          = newGameObject->AddComponent<GameEngine::Components::Animator>();

    animator.startupAnimationClipName = "noname";
    rendererComponent.AddModel(modelFile.GetAbsolutePath().string());

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

    RefreshAnimationList();
}

void AnimationViewerFrame::CreateGameObjectBasedOnPrefab(const GameEngine::File& path)
{
    auto prefabObject = GameEngine::SceneReader(canvas->GetScene()).loadPrefab(path);
    if (prefabObject)
    {
        if (prefabObject->GetChildren().empty())
        {
            wxMessageBox("Prefab load error. Prefab must contain exactly one root child object.", "Warning",
                         wxOK | wxICON_WARNING);
            return;
        }

        auto& go               = prefabObject->GetChildren().front();
        auto animator          = go->GetComponent<GameEngine::Components::Animator>();
        auto rendererComponent = go->GetComponent<GameEngine::Components::RendererComponent>();

        if (animator and rendererComponent)
        {
            auto files    = rendererComponent->GetFiles();
            auto fileIter = files.find(GameEngine::LevelOfDetail::L1);
            if (fileIter != files.end())
            {
                currentGameObject.emplace(CurrentGameObject{.gameObjectId      = go->GetId(),
                                                            .currentModelFile  = fileIter->second,
                                                            .animator          = *animator,
                                                            .rendererComponent = *rendererComponent});

                RefreshAnimationList();
                return;
            }
            else
            {
                LOG_ERROR << "Model file not exist";
                wxMessageBox("Model file not exist", "Warning", wxOK | wxICON_WARNING);
            }
        }
        else
        {
            std::stringstream o;
            o << "Components get failure animator=" << Utils::BoolToString(animator)
              << " rendererComponent=" << Utils::BoolToString(rendererComponent);
            wxMessageBox(o.str(), "Warning", wxOK | wxICON_WARNING);
            LOG_ERROR << o.str();
        }

        LOG_DEBUG << "Remove object after failures";
        auto& scene = canvas->GetScene();
        scene.RemoveGameObject(*go);
    }
}

void AnimationViewerFrame::RefreshAnimationList()
{
    animList->Clear();
    if (currentGameObject)
    {
        const auto& animationClips = currentGameObject->animator.getAnimationClips();
        for (const auto& [name, _] : animationClips)
        {
            animList->Append(name);
        }
    }
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
    {
        wxMessageBox("Model not set", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    wxString selected = animList->GetStringSelection();
    if (selected.IsEmpty())
    {
        wxMessageBox("No animation selected!", "Export", wxOK | wxICON_WARNING);
        return;
    }

    wxFileDialog saveFileDialog(this, "Save animation to file",
                                currentGameObject->currentModelFile->GetAbsolutePath().parent_path().string(), selected + ".xml",
                                "Animation files (*.xml)|*.xml|All files (*.*)|*.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString path = saveFileDialog.GetPath();

    LOG_DEBUG << "Exporting animation " << selected << " to file: " << path;

    auto model = currentGameObject->rendererComponent.GetModelWrapper().Get();
    if (model)
    {
        auto maybeRootJoint = model->getRootJoint();
        const auto& clips   = currentGameObject->animator.getAnimationClips();
        auto iter           = clips.find(selected.ToStdString());

        if (not maybeRootJoint)
        {
            wxMessageBox("Model: rootJoint not exist", "Warning", wxOK | wxICON_WARNING);
            return;
        }

        if (iter == clips.end())
        {
            wxMessageBox("Clip not found", "Warning", wxOK | wxICON_WARNING);
            return;
        }

        GameEngine::Animation::ExportAnimationClipToFile(path.ToStdString(), iter->second.clip, *maybeRootJoint,
                                                         wxFileName(saveFileDialog.GetFilename()).GetName().ToStdString());
    }

    wxMessageBox("Exported " + selected + " to " + path, "Export", wxOK | wxICON_INFORMATION);
}

void AnimationViewerFrame::OnExportAll(wxCommandEvent& event)
{
    if (not currentGameObject)
    {
        wxMessageBox("Model not set", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    wxDirDialog dlg(this, "Export all animations to directory", GetStartupDialogPathBasedOnCurrent(),
                    wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON);

    int exportedCount = 0;
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        exportedCount = OnExportAll(path.ToStdString());
    }

    auto string = exportedCount > 0 ? "Exported sucessfully " + std::to_string(exportedCount) : "Export error, 0 clips exported";
    wxMessageBox(string, "Export", wxOK | wxICON_INFORMATION);
}

int AnimationViewerFrame::OnExportAll(const std::string& path)
{
    int exportedCount = 0;
    auto model        = currentGameObject->rendererComponent.GetModelWrapper().Get();
    if (model)
    {
        auto maybeRootJoint = model->getRootJoint();
        const auto& clips   = currentGameObject->animator.getAnimationClips();
        if (not maybeRootJoint)
        {
            wxMessageBox("Model: rootJoint not exist", "Warning", wxOK | wxICON_WARNING);
            return 0;
        }

        for (const auto& [name, info] : clips)
        {
            GameEngine::Animation::ExportAnimationClipToFile(path + "/" + name + ".xml", info.clip, *maybeRootJoint, name);
            ++exportedCount;
        }
    }

    return exportedCount;
}

void AnimationViewerFrame::CreatePrefab(wxCommandEvent& event)
{
    if (not currentGameObject)
    {
        wxMessageBox("Model not set", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    auto isModelInProjectDir = Utils::IsFileExistsInDir(ProjectManager::GetInstance().GetProjectPath().string(),
                                                        currentGameObject->currentModelFile->GetAbsolutePath().string());
    if (not isModelInProjectDir)
    {
        int answer = wxMessageBox("Model file is outside the project. To created prefab should be imported. Do you want import?",
                                  "Confirmation", wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES)
        {
            ImportCurrentObject();
        }
        else
        {
            return;
        }
    }

    auto& scene = canvas->GetScene();
    if (auto maybeGo = scene.GetGameObject(currentGameObject->gameObjectId.value()))
    {
        wxFileDialog fileDialog(this, "Choose prefab file", GetStartupDialogPathBasedOnCurrent(), maybeGo->GetName() + ".prefab",
                                "Pliki prefabow (*.prefab)|*.prefab", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        if (fileDialog.ShowModal() == wxID_CANCEL)
            return;

        const auto& clips = currentGameObject->animator.getAnimationClips();
        if (not clips.empty())
        {
            int answer = wxMessageBox("Would you like set startup animation?", "Confirmation", wxYES_NO | wxICON_QUESTION);

            if (answer == wxYES)
            {
                wxArrayString animNames;
                for (const auto& [name, _] : clips)
                    animNames.Add(name);

                wxSingleChoiceDialog animDialog(this, "Choose startup animation", "Startup Animation", animNames);

                if (animDialog.ShowModal() == wxID_OK)
                {
                    wxString chosenAnim                                  = animDialog.GetStringSelection();
                    currentGameObject->animator.startupAnimationClipName = chosenAnim.ToStdString();
                    LOG_DEBUG << "Startup animation chosen: " << chosenAnim.ToStdString();
                }
            }
        }

        wxString path = fileDialog.GetPath();
        GameEngine::createAndSavePrefab(GameEngine::File{path.c_str()}, *maybeGo);
    }
}

void AnimationViewerFrame::ImportCurrentObject()
{
    if (not currentGameObject)
    {
        wxMessageBox("Model not set", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    wxFileDialog fileDialog(this, "Choose prefab file", ProjectManager::GetInstance().GetProjectPath().string(),
                            currentGameObject->currentModelFile->GetFilename(), "Wszystkie pliki (*.*)|*.*",
                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (fileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString targetPath = fileDialog.GetPath();

    auto targetFilename = Utils::CopyFileToDirectory(currentGameObject->currentModelFile->GetAbsolutePath(),
                                                     wxFileName(targetPath).GetPath().ToStdString());

    if (not targetFilename)
    {
        wxMessageBox("Import error", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    const auto& meshes = currentGameObject->rendererComponent.GetModelWrapper().Get()->GetMeshes();
    for (const auto& mesh : meshes)
    {
        const auto& material = mesh.GetMaterial();

        auto copyTexture = [this, &targetPath](auto texture)
        {
            if (texture)
            {
                auto textureFile = Utils::FindFile(texture->GetFile()->GetFilename(),
                                                   currentGameObject->currentModelFile->GetAbsolutePath().parent_path());

                const auto fsTargetPath = std::filesystem::path(wxFileName(targetPath).GetPath().ToStdString());

                if (not textureFile.empty())
                {
                    auto relPath = Utils::GetRelativePathToFile(
                        currentGameObject->currentModelFile->GetAbsolutePath().parent_path(), textureFile);
                    LOG_DEBUG << "relPath : " << relPath;

                    if (relPath)
                    {
                        auto targetTexturePath = fsTargetPath / relPath.value();
                        std::filesystem::create_directories(targetTexturePath);
                        auto copiedFile = Utils::CopyFileToDirectory(textureFile, targetTexturePath);
                    }
                    else
                    {
                        Utils::CopyFileToDirectory(textureFile, wxFileName(targetPath).GetPath().ToStdString());
                    }
                }
            }
        };

        copyTexture(material.diffuseTexture);
        copyTexture(material.normalTexture);
        copyTexture(material.specularTexture);
        copyTexture(material.ambientTexture);
        copyTexture(material.displacementTexture);
    }

    currentGameObject->currentModelFile                = targetFilename->string();
    currentGameObject->rendererComponent.fileName_LOD1 = targetFilename->string();

    if (not currentGameObject->animator.animationClips.empty())
    {
        std::filesystem::path targetClipsFolder =
            std::filesystem::path(wxFileName(targetPath).GetPath().ToStdString()) / "AnimationClips/";
        if (not std::filesystem::exists(targetClipsFolder))
        {
            std::filesystem::create_directories(targetClipsFolder);
        }

        for (auto& clip : currentGameObject->animator.animationClips)
        {
            clip.file = Utils::ChangeFileParentPath(clip.file.GetAbsolutePath(), targetClipsFolder);
        }

        OnExportAll(targetClipsFolder.string());
    }
}

void AnimationViewerFrame::SearchAndAddClipsFromDir(const std::string& path)
{
    if (not currentGameObject)
        return;

    if (not animList->IsEmpty())
    {
        int answer = wxMessageBox("List not empty, clear current anims?", "Confirmation", wxYES_NO | wxICON_QUESTION);

        if (answer == wxYES)
        {
            animList->Clear();
            currentGameObject->animator.clearAnimationClips();
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
            LOG_DEBUG << "Found animation file in subfolders add clip : " << file;
            if (auto animationName = GameEngine::Animation::IsAnimationClip(file))
            {
                currentGameObject->animator.AddAnimationClip(*animationName, GameEngine::File(file));
                animList->Append(*animationName);
            }
        }

        auto animationName = GameEngine::Animation::IsAnimationClip(files.front());
        if (animationName)
        {
            currentGameObject->animator.startupAnimationClipName = *animationName;
        }

        currentGameObject->animator.initAnimationClips();
    }
}

void AnimationViewerFrame::Clear()
{
    auto& scene = canvas->GetScene();
    scene.RemoveGameObject(currentGameObject->gameObjectId.value());
    currentGameObject.reset();
    animList->Clear();
}

std::string AnimationViewerFrame::GetStartupDialogPathBasedOnCurrent() const
{
    return currentGameObject->currentModelFile.has_value()
               ? currentGameObject->currentModelFile->GetAbsolutePath().parent_path().string()
               : ProjectManager::GetInstance().GetProjectPath().string();
}