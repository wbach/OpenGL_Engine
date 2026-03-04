#include "MaterialEditorFrame.h"

#include <GameEngine/Components/Lights/DirectionalLightComponent.h>
#include <GameEngine/Resources/ITextureLoader.h>
#include <GameEngine/Resources/Models/Material.h>
#include <GameEngine/Resources/Models/Primitives.h>
#include <GameEngine/Resources/Textures/GeneralTexture.h>
#include <Logger/Log.h>

#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>

#include "WxEditor/EngineRelated/GLCanvas.h"
#include "WxEditor/EngineRelated/WxScenesDef.h"
#include "WxEditor/ProjectManager.h"
#include "WxEditor/WxHelpers/EditorUitls.h"

namespace
{
std::filesystem::path GetDirectoryPath(const std::filesystem::path& p)
{
    if (std::filesystem::is_directory(p))
    {
        return p;
    }
    return p.parent_path();
}
}  // namespace

MaterialEditorFrame::MaterialEditorFrame(const std::optional<GameEngine::File>& maybeFile, const wxString& title,
                                         const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
    , initialFile{maybeFile}
    , lastDirPath{maybeFile ? GetDirectoryPath(maybeFile->GetAbsolutePath()) : ProjectManager::GetInstance().GetProjectPath()}

{
    wxInitAllImageHandlers();
    mainSplitter = new wxSplitterWindow(this, wxID_ANY);

    auto onStartupDone = [this, file = maybeFile]()
    {
        canvas->SetCameraStartupPosition(vec3(-3.0f, 0.0f, 0.0f), vec3(0, 0.0, 0));

        auto lightGo         = canvas->GetScene().CreateGameObject("Light");
        auto& lightComponent = lightGo->AddComponent<GameEngine::Components::DirectionalLightComponent>();
        lightGo->SetWorldPosition(vec3(1000, 1500, 1000));
        lightComponent.isDayNightCycleControlled = false;
        canvas->GetScene().AddGameObject(std::move(lightGo));

        Init();

        if (file)
        {
            LoadMaterial(file->GetAbsolutePath().string());
            SetStatusText(file->GetFilename());
        }
    };
    canvas = new GLCanvas(mainSplitter, onStartupDone, nullptr, WxEditor::EMPTY_SCENE);
}

void MaterialEditorFrame::Init()
{
    wxScrolledWindow* rightPanel =
        new wxScrolledWindow(mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);
    rightPanel->SetScrollRate(5, 5);

    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);

    baseColorBtn = AddColorPicker(rightPanel, rightSizer, "Base Color (Albedo)",
                                  [this](wxCommandEvent&)
                                  {
                                      vec3 picked               = PickColor(Color(currentMaterial.baseColor));
                                      currentMaterial.baseColor = vec4(picked, currentMaterial.baseColor.value.w);
                                      UpdateMaterialInComponent();
                                  });

    subsurfaceColorBtn = AddColorPicker(rightPanel, rightSizer, "Subsurface Color",
                                        [this](wxCommandEvent&)
                                        {
                                            currentMaterial.subsurfaceColor =
                                                PickColor(Color(vec4(currentMaterial.subsurfaceColor, 1.0f)));
                                            UpdateMaterialInComponent();
                                        });

    auto AddPBRSlider = [&](const wxString& label, float& target, float range = 100.f)
    {
        rightSizer->Add(new wxStaticText(rightPanel, wxID_ANY, label), 0, wxALL, 5);
        wxSlider* slider = new wxSlider(rightPanel, wxID_ANY, static_cast<int>(target * (100.f * (100.f / range))), 0, 100);
        rightSizer->Add(slider, 0, wxEXPAND | wxALL, 5);
        slider->Bind(wxEVT_SLIDER,
                     [this, &target, slider, range](wxCommandEvent&)
                     {
                         target = (slider->GetValue() / 100.f) * (range / 100.f);
                         UpdateMaterialInComponent();
                     });
        return slider;
    };

    metallicSlider      = AddPBRSlider("Metallic Factor:", currentMaterial.metallicFactor);
    roughnessSlider     = AddPBRSlider("Roughness Factor:", currentMaterial.roughnessFactor);
    occlusionSlider     = AddPBRSlider("Occlusion Strength:", currentMaterial.occlusionStrength);
    normalScaleSlider   = AddPBRSlider("Normal Scale:", currentMaterial.normalScale, 200.f);
    iorSlider           = AddPBRSlider("Index of Refraction:", currentMaterial.indexOfRefraction, 300.f);
    opacityCutoffSlider = AddPBRSlider("Opacity Cutoff:", currentMaterial.opacityCutoff);
    sssStrengthSlider   = AddPBRSlider("Subsurface Strength:", currentMaterial.subsurfaceStrength);
    uvScaleSlider       = AddPBRSlider("UV Tiled Scale:", currentMaterial.uvScale, 1000.f);

    rightSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "Material Flags:"), 0, wxALL, 5);

    auto AddFlagCheck = [&](const wxString& label, uint32_t flag)
    {
        wxCheckBox* cb = new wxCheckBox(rightPanel, wxID_ANY, label);
        cb->SetValue((currentMaterial.flags & flag) != 0);
        rightSizer->Add(cb, 0, wxLEFT | wxRIGHT, 10);
        cb->Bind(wxEVT_CHECKBOX,
                 [this, flag, cb](wxCommandEvent&)
                 {
                     if (cb->GetValue())
                         currentMaterial.flags |= flag;
                     else
                         currentMaterial.flags &= ~flag;
                     UpdateMaterialInComponent();
                 });
        return cb;
    };

    doubleSidedCheck = AddFlagCheck("Double Sided", MAT_DOUBLE_SIDED);
    alphaTestCheck   = AddFlagCheck("Alpha Test (Cutout)", MAT_ALPHA_TEST);
    alphaBlendCheck  = AddFlagCheck("Alpha Blend (Transparent)", MAT_ALPHA_BLEND);
    foliageCheck     = AddFlagCheck("Foliage Mode", MAT_FOLIAGE);
    unlitCheck       = AddFlagCheck("Unlit (No Light)", MAT_UNLIT);
    shadowCheck      = AddFlagCheck("Receive Shadows", MAT_RECEIVE_SHADOW);

    auto& scene         = canvas->GetScene();
    auto& textureLoader = scene.GetResourceManager().GetTextureLoader();

    auto AddTex = [&](const wxString& label, wxTextCtrl*& outCtrl, GameEngine::GeneralTexture*& texField)
    {
        AddTexturePicker(rightPanel, rightSizer, label, outCtrl,
                         [this, &texField, &textureLoader](const std::string& path)
                         {
                             texField = textureLoader.LoadTexture(path, GameEngine::TextureParameters{});
                             UpdateMaterialInComponent();
                         });
    };

    AddTex("Base Color Texture", baseColorPathCtrl, currentMaterial.baseColorTexture);
    AddTex("Normal Map", normalPathCtrl, currentMaterial.normalTexture);
    AddTex("Metallic Texture", metallicPathCtrl, currentMaterial.metallicTexture);
    AddTex("Roughness Texture", roughnessPathCtrl, currentMaterial.roughnessTexture);
    AddTex("Occlusion Texture", occlusionPathCtrl, currentMaterial.occlusionTexture);
    AddTex("Emission Texture", emissionPathCtrl, currentMaterial.emissionTexture);
    AddTex("Opacity Mask", opacityPathCtrl, currentMaterial.opacityTexture);
    AddTex("Displacement/Height", displacementPathCtrl, currentMaterial.displacementTexture);

    rightPanel->SetSizer(rightSizer);
    rightPanel->FitInside();

    auto size = GetSize();
    mainSplitter->SplitVertically(canvas, rightPanel, 3 * size.x / 4);

    previwGameObject = canvas->addPrimitive(GameEngine::PrimitiveType::Sphere);
    UpdateMaterialInComponent();

    CreateMainMenu();
    CreateStatusBar();

    timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &MaterialEditorFrame::OnTimer, this);
    timer->Start(16);
}

void MaterialEditorFrame::UpdateMaterialInComponent()
{
    if (previwGameObject)
    {
        if (auto rendererComponent = previwGameObject->GetComponent<GameEngine::Components::RendererComponent>())
        {
            if (auto model = rendererComponent->GetModelWrapper().Get())
            {
                if (!model->GetMeshes().empty())
                {
                    const auto& mesh = model->GetMeshes().front();
                    rendererComponent->UpdateCustomMaterial(mesh, currentMaterial);
                }
            }
        }
    }
}

void MaterialEditorFrame::LoadMaterial(const std::string& file)
{
    auto& scene         = canvas->GetScene();
    auto& textureLoader = scene.GetResourceManager().GetTextureLoader();

    currentMaterial = GameEngine::ParseMaterial(file, textureLoader);

    baseColorBtn->SetBackgroundColour(wxColour(currentMaterial.baseColor.value.x * 255, currentMaterial.baseColor.value.y * 255,
                                               currentMaterial.baseColor.value.z * 255));
    subsurfaceColorBtn->SetBackgroundColour(wxColour(currentMaterial.subsurfaceColor.x * 255,
                                                     currentMaterial.subsurfaceColor.y * 255,
                                                     currentMaterial.subsurfaceColor.z * 255));

    metallicSlider->SetValue(currentMaterial.metallicFactor * 100);
    roughnessSlider->SetValue(currentMaterial.roughnessFactor * 100);
    occlusionSlider->SetValue(currentMaterial.occlusionStrength * 100);
    normalScaleSlider->SetValue((currentMaterial.normalScale / 2.0f) * 100);
    iorSlider->SetValue((currentMaterial.indexOfRefraction / 3.0f) * 100);
    opacityCutoffSlider->SetValue(currentMaterial.opacityCutoff * 100);
    sssStrengthSlider->SetValue(currentMaterial.subsurfaceStrength * 100);
    uvScaleSlider->SetValue((currentMaterial.uvScale / 10.0f) * 100);

    doubleSidedCheck->SetValue((currentMaterial.flags & MAT_DOUBLE_SIDED) != 0);
    alphaTestCheck->SetValue((currentMaterial.flags & MAT_ALPHA_TEST) != 0);
    alphaBlendCheck->SetValue((currentMaterial.flags & MAT_ALPHA_BLEND) != 0);
    foliageCheck->SetValue((currentMaterial.flags & MAT_FOLIAGE) != 0);
    unlitCheck->SetValue((currentMaterial.flags & MAT_UNLIT) != 0);
    shadowCheck->SetValue((currentMaterial.flags & MAT_RECEIVE_SHADOW) != 0);

    auto setTexPath = [](wxTextCtrl* ctrl, GameEngine::GeneralTexture* tex)
    {
        if (ctrl)
        {
            if (tex && tex->GetFile())
                ctrl->SetValue(tex->GetFile()->GetDataRelativePath().generic_string());
            else
                ctrl->Clear();
        }
    };

    setTexPath(baseColorPathCtrl, currentMaterial.baseColorTexture);
    setTexPath(normalPathCtrl, currentMaterial.normalTexture);
    setTexPath(metallicPathCtrl, currentMaterial.metallicTexture);
    setTexPath(roughnessPathCtrl, currentMaterial.roughnessTexture);
    setTexPath(occlusionPathCtrl, currentMaterial.occlusionTexture);
    setTexPath(emissionPathCtrl, currentMaterial.emissionTexture);
    setTexPath(opacityPathCtrl, currentMaterial.opacityTexture);
    setTexPath(displacementPathCtrl, currentMaterial.displacementTexture);

    materialLoaded = true;
    canvas->Refresh();
    UpdateMaterialInComponent();
}

wxButton* MaterialEditorFrame::AddColorPicker(wxPanel* parent, wxBoxSizer* sizer, const wxString& labelText,
                                              std::function<void(wxCommandEvent&)> onChange)
{
    sizer->Add(new wxStaticText(parent, wxID_ANY, labelText), 0, wxALL, 5);
    wxButton* btn = new wxButton(parent, wxID_ANY, "Choose " + labelText);
    sizer->Add(btn, 0, wxEXPAND | wxALL, 5);
    btn->Bind(wxEVT_BUTTON, onChange);
    return btn;
}

vec3 MaterialEditorFrame::PickColor(const Color& currentColor)
{
    wxColourData data;
    data.SetColour(wxColour(currentColor.r(), currentColor.g(), currentColor.b()));
    wxColourDialog dlg(this, &data);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColour col = dlg.GetColourData().GetColour();
        canvas->Refresh();
        return vec3(col.Red() / 255.f, col.Green() / 255.f, col.Blue() / 255.f);
    }
    return vec3(currentColor.r(), currentColor.g(), currentColor.b());
}

wxBoxSizer* MaterialEditorFrame::AddTexturePicker(wxPanel* parent, wxBoxSizer* sizer, const wxString& labelText,
                                                  wxTextCtrl*& outTextCtrl,
                                                  std::function<void(const std::string&)> onTextureChanged)
{
    sizer->Add(new wxStaticText(parent, wxID_ANY, labelText), 0, wxALL, 5);
    wxBoxSizer* texSizer = new wxBoxSizer(wxHORIZONTAL);
    outTextCtrl          = new wxTextCtrl(parent, wxID_ANY);
    wxButton* browseBtn  = new wxButton(parent, wxID_ANY, "Browse...");
    texSizer->Add(outTextCtrl, 1, wxEXPAND | wxALL, 2);
    texSizer->Add(browseBtn, 0, wxALL, 2);
    sizer->Add(texSizer, 0, wxEXPAND | wxALL, 5);

    browseBtn->Bind(wxEVT_BUTTON,
                    [this, outTextCtrl, onTextureChanged](wxCommandEvent&)
                    {
                        wxFileDialog openFileDialog(this, _("Choose texture file"), lastDirPath.string(), "",
                                                    "Image files (*.png;*.jpg;*.tga)|*.png;*.jpg;*.tga|All files (*.*)|*.*",
                                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                        if (openFileDialog.ShowModal() == wxID_OK)
                        {
                            const auto& path = openFileDialog.GetPath().ToStdString();

                            outTextCtrl->SetValue(openFileDialog.GetPath());
                            onTextureChanged(path);
                            lastDirPath = GetDirectoryPath(path);
                            canvas->Refresh();
                        }
                    });
    return texSizer;
}

void MaterialEditorFrame::OnTimer(wxTimerEvent&)
{
    if (canvas)
        canvas->Refresh();
}

void MaterialEditorFrame::CreateMainMenu()
{
    wxMenuBar* menuBar = new wxMenuBar();
    wxMenu* fileMenu   = new wxMenu();
    fileMenu->Append(wxID_OPEN, "&Open Material...\tCtrl+O");
    fileMenu->Append(wxID_SAVE, "&Save Material...\tCtrl+S");
    fileMenu->Append(wxID_SAVEAS, "&Save Material as...\tCtrl+S");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "&Exit\tAlt+F4");
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &MaterialEditorFrame::OnOpenMaterial, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MaterialEditorFrame::OnSaveMaterial, this, wxID_SAVE);
    Bind(wxEVT_MENU, &MaterialEditorFrame::OnSaveMaterialAs, this, wxID_SAVEAS);
    Bind(wxEVT_MENU, &MaterialEditorFrame::OnExit, this, wxID_EXIT);
}

void MaterialEditorFrame::OnOpenMaterial(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Open Material file", lastDirPath.string(), "",
                                "Material files (*.material)|*.material|All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK)
    {
        const auto& path = openFileDialog.GetPath().ToStdString();
        LoadMaterial(path);
        lastDirPath = GetDirectoryPath(path);
    }
}

void MaterialEditorFrame::OnSaveMaterial(wxCommandEvent& e)
{
    if (initialFile and initialFile->exist())
    {
        GameEngine::SaveMaterial(currentMaterial, initialFile->GetAbsolutePath());
    }
    else
    {
        OnSaveMaterialAs(e);
    }
}

void MaterialEditorFrame::OnSaveMaterialAs(wxCommandEvent&)
{
    wxFileDialog fileDialog(this, "Save Material", ProjectManager::GetInstance().GetProjectPath().string(), "",
                            "Material files (*.material)|*.material", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (fileDialog.ShowModal() == wxID_OK)
    {
        const auto& path = fileDialog.GetPath().ToStdString();
        GameEngine::SaveMaterial(currentMaterial, fileDialog.GetPath().ToStdString());
        lastDirPath = GetDirectoryPath(path);
    }
}

void MaterialEditorFrame::OnExit(wxCommandEvent&)
{
    Close(true);
}
bool MaterialEditorFrame::IsMaterialLoaded() const
{
    return materialLoaded;
}
const GameEngine::Material& MaterialEditorFrame::GetMaterial() const
{
    return currentMaterial;
}