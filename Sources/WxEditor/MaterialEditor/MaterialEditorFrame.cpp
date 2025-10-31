#include "MaterialEditorFrame.h"

#include <GameEngine/Resources/ITextureLoader.h>
#include <GameEngine/Resources/Models/Material.h>
#include <GameEngine/Resources/Models/Primitives.h>
#include <GameEngine/Resources/Textures/GeneralTexture.h>
#include <Logger/Log.h>

#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>

#include "WxEditor/EngineRelated/GLCanvas.h"
#include "WxEditor/ProjectManager.h"
#include "WxEditor/WxHelpers/EditorUitls.h"

MaterialEditorFrame::MaterialEditorFrame(const std::optional<GameEngine::File>& maybeFile, const wxString& title,
                                         const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    wxInitAllImageHandlers();
    mainSplitter = new wxSplitterWindow(this, wxID_ANY);

    // Canvas renderujący preview materiału
    auto onStartupDone = [this, file = maybeFile]()
    {
        auto& camera = canvas->GetScene().GetCamera();
        camera.SetPosition(vec3(-3.0f, 0.0f, 0.0f));
        camera.LookAt(vec3(0, 0.0f, 0));
        camera.UpdateMatrix();
        Init();

        if (file)
        {
            LoadMaterial(file->GetAbsolutePath().string());
            SetStatusText(file->GetFilename());
        }
    };
    auto selectItem = [](uint32, bool) {};
    canvas          = new GLCanvas(mainSplitter, onStartupDone, selectItem, false);
}

void MaterialEditorFrame::Init()
{
    // Panel boczny z przewijaniem
    wxScrolledWindow* rightPanel =
        new wxScrolledWindow(mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);
    rightPanel->SetScrollRate(5, 5);  // ustawienie scroll rate (piksele)

    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);

    // --- Kolory ---
    ambientBtn = AddColorPicker(rightPanel, rightSizer, "Ambient",
                                [this](wxCommandEvent&)
                                {
                                    currentMaterial.ambient = PickColor(currentMaterial.ambient);
                                    UpdateMaterialInComponent();
                                });

    diffuseBtn = AddColorPicker(rightPanel, rightSizer, "Diffuse",
                                [this](wxCommandEvent&)
                                {
                                    currentMaterial.diffuse = PickColor(currentMaterial.diffuse);
                                    UpdateMaterialInComponent();
                                });

    specularBtn = AddColorPicker(rightPanel, rightSizer, "Specular",
                                 [this](wxCommandEvent&)
                                 {
                                     currentMaterial.specular = PickColor(currentMaterial.specular);
                                     UpdateMaterialInComponent();
                                 });

    // --- Roughness / ShineDamper ---
    rightSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "Shine Damper:"), 0, wxALL, 5);
    roughnessSlider = new wxSlider(rightPanel, wxID_ANY, static_cast<int>(currentMaterial.shineDamper * 100), 0, 100);
    rightSizer->Add(roughnessSlider, 0, wxEXPAND | wxALL, 5);
    roughnessSlider->Bind(wxEVT_SLIDER, &MaterialEditorFrame::OnRoughnessChanged, this);

    // --- Reflectivity ---
    rightSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "Reflectivity:"), 0, wxALL, 5);
    reflectivitySlider = new wxSlider(rightPanel, wxID_ANY, static_cast<int>(currentMaterial.reflectivity * 100), 0, 100);
    rightSizer->Add(reflectivitySlider, 0, wxEXPAND | wxALL, 5);
    reflectivitySlider->Bind(wxEVT_SLIDER,
                             [this](wxCommandEvent&)
                             {
                                 currentMaterial.reflectivity = reflectivitySlider->GetValue() / 100.f;
                                 canvas->Refresh();
                                 UpdateMaterialInComponent();
                             });

    // --- Index of Refraction ---
    rightSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "Index of Refraction:"), 0, wxALL, 5);
    iorSlider = new wxSlider(rightPanel, wxID_ANY, static_cast<int>(currentMaterial.indexOfRefraction * 100), 0, 500);
    rightSizer->Add(iorSlider, 0, wxEXPAND | wxALL, 5);
    iorSlider->Bind(wxEVT_SLIDER,
                    [this](wxCommandEvent&)
                    {
                        currentMaterial.indexOfRefraction = iorSlider->GetValue() / 100.f;
                        canvas->Refresh();
                        UpdateMaterialInComponent();
                    });

    // --- Transparency ---
    transparencyCheck = new wxCheckBox(rightPanel, wxID_ANY, "Transparency");
    transparencyCheck->SetValue(currentMaterial.isTransparency);
    rightSizer->Add(transparencyCheck, 0, wxALL, 5);
    transparencyCheck->Bind(wxEVT_CHECKBOX,
                            [this](wxCommandEvent&)
                            {
                                currentMaterial.isTransparency = transparencyCheck->GetValue();
                                canvas->Refresh();
                                UpdateMaterialInComponent();
                            });

    // --- Use Fake Lighting ---
    fakeLightingCheck = new wxCheckBox(rightPanel, wxID_ANY, "Use Fake Lighting");
    fakeLightingCheck->SetValue(currentMaterial.useFakeLighting);
    rightSizer->Add(fakeLightingCheck, 0, wxALL, 5);
    fakeLightingCheck->Bind(wxEVT_CHECKBOX,
                            [this](wxCommandEvent&)
                            {
                                currentMaterial.useFakeLighting = fakeLightingCheck->GetValue();
                                canvas->Refresh();
                                UpdateMaterialInComponent();
                            });

    // --- Tekstury ---
    auto& scene         = canvas->GetScene();
    auto& textureLoader = scene.GetResourceManager().GetTextureLoader();

    AddTexturePicker(rightPanel, rightSizer, "Diffuse Texture", diffusePathCtrl,
                     [this, &textureLoader](const std::string& path)
                     {
                         currentMaterial.diffuseTexture = textureLoader.LoadTexture(path, GameEngine::TextureParameters{});
                         UpdateMaterialInComponent();
                     });

    AddTexturePicker(rightPanel, rightSizer, "Ambient Texture", ambientPathCtrl,
                     [this, &textureLoader](const std::string& path)
                     {
                         currentMaterial.ambientTexture = textureLoader.LoadTexture(path, GameEngine::TextureParameters{});
                         UpdateMaterialInComponent();
                     });

    AddTexturePicker(rightPanel, rightSizer, "Specular Texture", specularPathCtrl,
                     [this, &textureLoader](const std::string& path)
                     {
                         currentMaterial.specularTexture = textureLoader.LoadTexture(path, GameEngine::TextureParameters{});
                         UpdateMaterialInComponent();
                     });

    AddTexturePicker(rightPanel, rightSizer, "Normal Texture", normalPathCtrl,
                     [this, &textureLoader](const std::string& path)
                     {
                         currentMaterial.normalTexture = textureLoader.LoadTexture(path, GameEngine::TextureParameters{});
                         UpdateMaterialInComponent();
                     });

    AddTexturePicker(rightPanel, rightSizer, "Displacement Texture", displacementPathCtrl,
                     [this, &textureLoader](const std::string& path)
                     {
                         currentMaterial.displacementTexture = textureLoader.LoadTexture(path, GameEngine::TextureParameters{});
                         UpdateMaterialInComponent();
                     });

    rightPanel->SetSizer(rightSizer);
    rightPanel->FitInside();
    rightPanel->SetScrollRate(5, 5);

    // --- Splitter ---
    auto size = GetSize();
    mainSplitter->SplitVertically(canvas, rightPanel, 3 * size.x / 4);

    previwGameObject = canvas->addPrimitive(GameEngine::PrimitiveType::Sphere);
    UpdateMaterialInComponent();

    CreateMainMenu();
    CreateStatusBar();
    SetStatusText("Material Editor ready");

    // Timer do odświeżania GLCanvas
    timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &MaterialEditorFrame::OnTimer, this);
    timer->Start(16);
}

wxButton* MaterialEditorFrame::AddColorPicker(wxPanel* parent, wxBoxSizer* sizer, const wxString& labelText,
                                              std::function<void(wxCommandEvent&)> onChange)
{
    sizer->Add(new wxStaticText(parent, wxID_ANY, labelText), 0, wxALL, 5);
    wxButton* btn = new wxButton(parent, wxID_ANY, "Choose " + labelText);
    sizer->Add(btn, 0, wxEXPAND | wxALL, 5);

    // Bind z lambda, która wywoła std::function
    btn->Bind(wxEVT_BUTTON, onChange);

    return btn;
}

// Timer do odświeżania podglądu
void MaterialEditorFrame::OnTimer(wxTimerEvent&)
{
    if (canvas)
        canvas->Refresh();
}

vec3 MaterialEditorFrame::PickColor(const Color& currentColor)
{
    wxColourData data;
    // ustawienie aktualnego koloru jako startowego
    data.SetColour(wxColour(currentColor.r(), currentColor.g(), currentColor.b()));
    wxColourDialog dlg(this, &data);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColour col = dlg.GetColourData().GetColour();
        vec3 result(col.Red() / 255.f, col.Green() / 255.f, col.Blue() / 255.f);
        canvas->Refresh();
        return result;
    }
    return vec3(0.f);
}

wxBoxSizer* MaterialEditorFrame::AddTexturePicker(wxPanel* parent, wxBoxSizer* sizer, const wxString& labelText,
                                                  wxTextCtrl*& outTextCtrl,
                                                  std::function<void(const std::string&)> onTextureChanged)
{
    // Label
    sizer->Add(new wxStaticText(parent, wxID_ANY, labelText), 0, wxALL, 5);

    // HBox z TextCtrl i Browse button
    wxBoxSizer* texSizer = new wxBoxSizer(wxHORIZONTAL);
    outTextCtrl          = new wxTextCtrl(parent, wxID_ANY);
    wxButton* browseBtn  = new wxButton(parent, wxID_ANY, "Browse...");
    texSizer->Add(outTextCtrl, 1, wxEXPAND | wxALL, 2);
    texSizer->Add(browseBtn, 0, wxALL, 2);
    sizer->Add(texSizer, 0, wxEXPAND | wxALL, 5);

    // Obsługa przycisku Browse
    browseBtn->Bind(wxEVT_BUTTON,
                    [this, outTextCtrl, onTextureChanged](wxCommandEvent&)
                    {
                        wxFileDialog openFileDialog(this, _("Choose texture file"),
                                                    ProjectManager::GetInstance().GetProjectPath().string(), "",
                                                    "Image files (*.png;*.jpg;*.tga)|*.png;*.jpg;*.tga|All files (*.*)|*.*",
                                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST);

                        if (openFileDialog.ShowModal() == wxID_CANCEL)
                            return;

                        outTextCtrl->SetValue(openFileDialog.GetPath());
                        onTextureChanged(openFileDialog.GetPath().ToStdString());
                        canvas->Refresh();
                    });

    return texSizer;
}

// Zmiana shineDamper / roughness
void MaterialEditorFrame::OnRoughnessChanged(wxCommandEvent&)
{
    currentMaterial.shineDamper = roughnessSlider->GetValue() / 100.f;
    LOG_DEBUG << "ShineDamper: " << currentMaterial.shineDamper;
    canvas->Refresh();
}

void MaterialEditorFrame::CreateMainMenu()
{
    wxMenuBar* menuBar = new wxMenuBar();

    // --- File Menu ---
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_OPEN, "&Open Material...\tCtrl+O", "Open a material file");
    fileMenu->Append(wxID_SAVE, "&Save Material...\tCtrl+S", "Save a material file");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt+F4", "Exit the application");
    menuBar->Append(fileMenu, "&File");

    SetMenuBar(menuBar);

    // Bind zdarzenia
    Bind(wxEVT_MENU, &MaterialEditorFrame::OnOpenMaterial, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MaterialEditorFrame::OnSaveMaterial, this, wxID_SAVE);
    Bind(wxEVT_MENU, &MaterialEditorFrame::OnExit, this, wxID_EXIT);
}

void MaterialEditorFrame::OnOpenMaterial(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Open Material file", ProjectManager::GetInstance().GetProjectPath().string(), "",
                                "Material files (*.material)|*.material|Material files (*.json)|*.json|All files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    std::string path = openFileDialog.GetPath().ToStdString();
    LOG_DEBUG << "Load material " << path;
    LoadMaterial(path);
    LOG_DEBUG << "Load material done. " << path;

    UpdateMaterialInComponent();
    SetStatusText(GameEngine::File(path).GetFilename());

    LOG_DEBUG << "OnOpenMaterial done. " << path;
}

void MaterialEditorFrame::OnSaveMaterial(wxCommandEvent&)
{
    wxFileDialog fileDialog(this, "Wybierz plik", ProjectManager::GetInstance().GetProjectPath().string(), "",
                            "Material files (*.material)|*.material|Material files (*.json)|*.json|All files (*.*)|*.*",
                            wxFD_SAVE);

    if (fileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString path = fileDialog.GetPath();
    GameEngine::SaveMaterial(currentMaterial, path.ToStdString());
    SetStatusText(GameEngine::File(path).GetFilename());
}

void MaterialEditorFrame::UpdateMaterialInComponent()
{
    if (previwGameObject)
    {
        if (auto rendererComponent = previwGameObject->GetComponent<GameEngine::Components::RendererComponent>())
        {
            if (auto model = rendererComponent->GetModelWrapper().Get())
            {
                if (model->GetMeshes().size() > 0)
                {
                    LOG_DEBUG << "UpdateCustomMaterial";
                    const auto& mesh = model->GetMeshes().front();
                    rendererComponent->UpdateCustomMaterial(mesh, currentMaterial);
                }
            }
        }
    }
}

void MaterialEditorFrame::OnExit(wxCommandEvent&)
{
    Close(true);
}

void MaterialEditorFrame::LoadMaterial(const std::string& file)
{
    auto& scene = canvas->GetScene();

    auto& textureLoader = scene.GetResourceManager().GetTextureLoader();

    LOG_DEBUG << "Parse material";
    currentMaterial = GameEngine::ParseMaterial(file, textureLoader);
    LOG_DEBUG << "Parse material done";

    // --- Kolory ---
    ambientBtn->SetBackgroundColour(
        wxColour(currentMaterial.ambient.r(), currentMaterial.ambient.g(), currentMaterial.ambient.b()));
    diffuseBtn->SetBackgroundColour(
        wxColour(currentMaterial.diffuse.r(), currentMaterial.diffuse.g(), currentMaterial.diffuse.b()));
    specularBtn->SetBackgroundColour(
        wxColour(currentMaterial.specular.r(), currentMaterial.specular.g(), currentMaterial.specular.b()));

    // --- Slidery ---
    roughnessSlider->SetValue(static_cast<int>(currentMaterial.shineDamper * 100));
    reflectivitySlider->SetValue(static_cast<int>(currentMaterial.reflectivity * 100));
    iorSlider->SetValue(static_cast<int>(currentMaterial.indexOfRefraction * 100));

    // --- Checkboxy ---
    transparencyCheck->SetValue(currentMaterial.isTransparency);
    fakeLightingCheck->SetValue(currentMaterial.useFakeLighting);

    // --- Tekstury ---
    if (currentMaterial.diffuseTexture)
        diffusePathCtrl->SetValue(currentMaterial.diffuseTexture->GetFile()->GetDataRelativePath().string());
    else
        diffusePathCtrl->Clear();

    if (currentMaterial.ambientTexture)
        ambientPathCtrl->SetValue(currentMaterial.ambientTexture->GetFile()->GetDataRelativePath().string());
    else
        ambientPathCtrl->Clear();

    if (currentMaterial.specularTexture)
        specularPathCtrl->SetValue(currentMaterial.specularTexture->GetFile()->GetDataRelativePath().string());
    else
        specularPathCtrl->Clear();

    if (currentMaterial.normalTexture)
        normalPathCtrl->SetValue(currentMaterial.normalTexture->GetFile()->GetDataRelativePath().string());
    else
        normalPathCtrl->Clear();

    if (currentMaterial.displacementTexture)
        displacementPathCtrl->SetValue(currentMaterial.displacementTexture->GetFile()->GetDataRelativePath().string());
    else
        displacementPathCtrl->Clear();

    materialLoaded = true;
    canvas->Refresh();
    UpdateMaterialInComponent();
    LOG_DEBUG << "Material loaded from: " << file;
}
bool MaterialEditorFrame::IsMaterialLoaded() const
{
    return materialLoaded;
}
const GameEngine::Material& MaterialEditorFrame::GetMaterial() const
{
    return currentMaterial;
}
