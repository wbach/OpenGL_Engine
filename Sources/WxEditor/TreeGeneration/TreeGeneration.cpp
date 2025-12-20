#include "TreeGeneration.h"

#include <GameEngine/Components/Renderer/Trees/Tree.h>
#include <GameEngine/Components/Renderer/Trees/TreeMeshBuilder.h>
#include <GameEngine/Components/Renderer/Trees/TreeRendererComponent.h>
#include <GameEngine/Resources/ITextureLoader.h>
#include <GameEngine/Resources/Models/Material.h>
#include <GameEngine/Resources/Models/MaterialPresets.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>
#include <wx/wx.h>

#include <optional>

#include "WxEditor/WxHelpers/LoadingDialog.h"

namespace
{
struct TreeGenerationParams
{
    size_t attractorsCount        = 400;
    float crownRadius             = 10.f;
    float trunkMaterialTiledScale = 2.f;
    float maxDistance{5.f};
    float minDistance{1.f};
    float segmentLength{0.3f};
    float crownYOffset{0.f};
    vec3 rootPosition{0.f};
    vec3 rootDirection{0.f, 1.f, 0.f};
    std::string trunkMaterialBaseColorTexture        = "Textures/Tree/trunk/light-tree-bark_albedo.png";
    std::string trunkMaterialAmbientOcclusionTexture = "Textures/Tree/trunk/light-tree-bark_ao.png";
    std::string trunkMaterialDisplacementTexture     = "Textures/Tree/trunk/light-tree-bark_albedo.png";
    std::string trunkMaterialMetallicTexture         = "Textures/Tree/trunk/light-tree-bark_metallic.png";
    std::string trunkMaterialNormalTexture           = "Textures/Tree/trunk/light-tree-bark_normal-ogl.png";
    std::string trunkMaterialRoughnessTexture        = "Textures/Tree/trunk/light-tree-bark_roughness.png";
    std::string leafMaterialBaseColorTexture         = "Textures/Tree/Leafs/LeafSet024_2K-PNG_Color.png";
    std::string leafMaterialOpacityTexture           = "Textures/Tree/Leafs/LeafSet024_2K-PNG_Opacity.png";
    std::string leafMaterialRoughnessTexture         = "Textures/Tree/Leafs/LeafSet024_2K-PNG_Roughness.png";
    std::string leafMaterialNormalTexture            = "Textures/Tree/Leafs/LeafSet024_2K-PNG_NormalGL.png";

    GameEngine::TreeMeshBuilder::EntryParameters meshBuilderParams;
};

TreeGenerationParams treeBuidlerParams;

std::optional<TreeGenerationParams> EditTreeGenerationParams(wxWindow* parent, const TreeGenerationParams& initial)
{
    TreeGenerationParams defaults{};
    wxString lastDirectory;

    wxDialog dlg(parent, wxID_ANY, "Tree Generation Parameters", wxDefaultPosition, wxDefaultSize,
                 wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);
    auto* grid      = new wxFlexGridSizer(2, 8, 8);
    grid->AddGrowableCol(1, 1);

    auto addFloat = [&](const wxString& label, float value)
    {
        grid->Add(new wxStaticText(&dlg, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);
        auto* ctrl = new wxTextCtrl(&dlg, wxID_ANY, wxString::Format("%.4f", value));
        grid->Add(ctrl, 1, wxEXPAND);
        return ctrl;
    };

    auto addSizeT = [&](const wxString& label, size_t value)
    {
        grid->Add(new wxStaticText(&dlg, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);
        auto* ctrl = new wxSpinCtrl(&dlg, wxID_ANY);
        ctrl->SetRange(0, 1'000'000);
        ctrl->SetValue(static_cast<int>(value));
        grid->Add(ctrl, 1, wxEXPAND);
        return ctrl;
    };

    auto addFile = [&](const wxString& label, const std::string& path)
    {
        grid->Add(new wxStaticText(&dlg, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);

        auto* picker = new wxFilePickerCtrl(&dlg, wxID_ANY, "", "Select file", "*.*", wxDefaultPosition, wxDefaultSize,
                                            wxFLP_USE_TEXTCTRL | wxFLP_OPEN);

        if (!path.empty())
            picker->SetPath(wxString::FromUTF8(path.c_str()));

        picker->Bind(wxEVT_FILEPICKER_CHANGED, [&](wxFileDirPickerEvent& e) { lastDirectory = wxPathOnly(e.GetPath()); });

        grid->Add(picker, 1, wxEXPAND);
        return picker;
    };
    struct Vec3Controls
    {
        wxTextCtrl* x;
        wxTextCtrl* y;
        wxTextCtrl* z;
    };

    auto addVec3 = [&](const wxString& label, const glm::vec3& value)
    {
        grid->Add(new wxStaticText(&dlg, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);

        auto* row = new wxBoxSizer(wxHORIZONTAL);

        auto makeFloat = [&](float v)
        {
            auto* t = new wxTextCtrl(&dlg, wxID_ANY, wxString::Format("%.4f", v), wxDefaultPosition, wxSize(70, -1));
            row->Add(t, 1, wxRIGHT, 4);
            return t;
        };

        auto* x = makeFloat(value.x);
        auto* y = makeFloat(value.y);
        auto* z = makeFloat(value.z);

        grid->Add(row, 1, wxEXPAND);

        return Vec3Controls{x, y, z};
    };

    // --- Basic params ---
    auto* attractorsCount = addSizeT("Attractors Count", initial.attractorsCount);
    auto* crownRadius     = addFloat("Crown Radius", initial.crownRadius);
    auto* tileScale       = addFloat("Trunk Tiled Scale", initial.trunkMaterialTiledScale);
    auto* maxDistance     = addFloat("SCA Max distance", initial.maxDistance);
    auto* minDistance     = addFloat("SCA Min distance", initial.minDistance);
    auto* segmentLength   = addFloat("Trunk segment scale", initial.segmentLength);
    auto* crownYOffset    = addFloat("Crown y offset", initial.crownYOffset);

    auto rootPosition  = addVec3("Root poosition", initial.rootPosition);
    auto rootDirection = addVec3("Root direction", initial.rootDirection);

    struct EntryControls
    {
        wxSpinCtrl* radialSegments;
        wxTextCtrl* leafheightThreshold;
        wxTextCtrl* leafRandomFactor;
        wxSpinCtrl* leafsPerBranch;
        wxTextCtrl* leafSpread;
        wxTextCtrl* minBranchRadius;
        wxTextCtrl* maxBranchRadius;
        wxSpinCtrl* textureAtlasSize;
    };

    EntryControls meshControls;

    meshControls.radialSegments      = addSizeT("Radial Segments", initial.meshBuilderParams.radialSegments);
    meshControls.leafheightThreshold = addFloat("Leaf Height Threshold", initial.meshBuilderParams.leafheightTreshold);
    meshControls.leafRandomFactor    = addFloat("Leaf Random Factor", initial.meshBuilderParams.leafRandomFactor);
    meshControls.leafsPerBranch      = addSizeT("Leafs Per Branch", initial.meshBuilderParams.leafsPerBranch);
    meshControls.leafSpread          = addFloat("Leaf Spread", initial.meshBuilderParams.leafSpread);
    meshControls.minBranchRadius     = addFloat("Min Branch Radius", initial.meshBuilderParams.minBranchRadius);
    meshControls.maxBranchRadius     = addFloat("Max Branch Radius", initial.meshBuilderParams.maxBranchRadius);
    meshControls.textureAtlasSize    = addSizeT("Texture Atlas Size", initial.meshBuilderParams.textureAtlasSize);

    // --- Trunk textures ---
    auto* trunkAlbedo    = addFile("Trunk Albedo", initial.trunkMaterialBaseColorTexture);
    auto* trunkAO        = addFile("Trunk AO", initial.trunkMaterialAmbientOcclusionTexture);
    auto* trunkDisp      = addFile("Trunk Displacement", initial.trunkMaterialDisplacementTexture);
    auto* trunkMetallic  = addFile("Trunk Metallic", initial.trunkMaterialMetallicTexture);
    auto* trunkNormal    = addFile("Trunk Normal", initial.trunkMaterialNormalTexture);
    auto* trunkRoughness = addFile("Trunk Roughness", initial.trunkMaterialRoughnessTexture);

    // --- Leaf textures ---
    auto* leafAlbedo    = addFile("Leaf Albedo", initial.leafMaterialBaseColorTexture);
    auto* leafOpacity   = addFile("Leaf Opacity", initial.leafMaterialOpacityTexture);
    auto* leafRoughness = addFile("Leaf Roughness", initial.leafMaterialRoughnessTexture);
    auto* leafNormal    = addFile("Leaf Normal", initial.leafMaterialNormalTexture);

    mainSizer->Add(grid, 1, wxALL | wxEXPAND, 10);
    auto* btnSizer   = new wxBoxSizer(wxHORIZONTAL);
    auto* restoreBtn = new wxButton(&dlg, wxID_ANY, "Restore Defaults");
    auto* okBtn      = new wxButton(&dlg, wxID_OK);
    auto* cancelBtn  = new wxButton(&dlg, wxID_CANCEL);

    btnSizer->Add(restoreBtn, 0, wxRIGHT, 10);
    btnSizer->AddStretchSpacer();
    btnSizer->Add(okBtn, 0, wxRIGHT, 5);
    btnSizer->Add(cancelBtn, 0);

    mainSizer->Add(btnSizer, 0, wxALL | wxEXPAND, 10);
    dlg.SetSizerAndFit(mainSizer);
    dlg.CentreOnParent();

    restoreBtn->Bind(
        wxEVT_BUTTON,
        [&](wxCommandEvent&)
        {
            // --- Basic floats / size_t ---
            attractorsCount->SetValue(static_cast<int>(defaults.attractorsCount));
            crownRadius->SetValue(wxString::Format("%.4f", defaults.crownRadius));
            tileScale->SetValue(wxString::Format("%.4f", defaults.trunkMaterialTiledScale));
            maxDistance->SetValue(wxString::Format("%.4f", defaults.maxDistance));
            minDistance->SetValue(wxString::Format("%.4f", defaults.minDistance));
            segmentLength->SetValue(wxString::Format("%.4f", defaults.segmentLength));
            crownYOffset->SetValue(wxString::Format("%.4f", defaults.crownYOffset));

            // --- Vec3 ---
            rootPosition.x->SetValue(wxString::Format("%.4f", defaults.rootPosition.x));
            rootPosition.y->SetValue(wxString::Format("%.4f", defaults.rootPosition.y));
            rootPosition.z->SetValue(wxString::Format("%.4f", defaults.rootPosition.z));

            rootDirection.x->SetValue(wxString::Format("%.4f", defaults.rootDirection.x));
            rootDirection.y->SetValue(wxString::Format("%.4f", defaults.rootDirection.y));
            rootDirection.z->SetValue(wxString::Format("%.4f", defaults.rootDirection.z));

            // --- Trunk textures ---
            trunkAlbedo->SetPath(wxString::FromUTF8(defaults.trunkMaterialBaseColorTexture.c_str()));
            trunkAO->SetPath(wxString::FromUTF8(defaults.trunkMaterialAmbientOcclusionTexture.c_str()));
            trunkDisp->SetPath(wxString::FromUTF8(defaults.trunkMaterialDisplacementTexture.c_str()));
            trunkMetallic->SetPath(wxString::FromUTF8(defaults.trunkMaterialMetallicTexture.c_str()));
            trunkNormal->SetPath(wxString::FromUTF8(defaults.trunkMaterialNormalTexture.c_str()));
            trunkRoughness->SetPath(wxString::FromUTF8(defaults.trunkMaterialRoughnessTexture.c_str()));

            // --- Leaf textures ---
            leafAlbedo->SetPath(wxString::FromUTF8(defaults.leafMaterialBaseColorTexture.c_str()));
            leafOpacity->SetPath(wxString::FromUTF8(defaults.leafMaterialOpacityTexture.c_str()));
            leafRoughness->SetPath(wxString::FromUTF8(defaults.leafMaterialRoughnessTexture.c_str()));
            leafNormal->SetPath(wxString::FromUTF8(defaults.leafMaterialNormalTexture.c_str()));

            // --- MeshBuilder EntryParameters ---
            meshControls.radialSegments->SetValue(defaults.meshBuilderParams.radialSegments);
            meshControls.leafheightThreshold->SetValue(wxString::Format("%.4f", defaults.meshBuilderParams.leafheightTreshold));
            meshControls.leafRandomFactor->SetValue(wxString::Format("%.4f", defaults.meshBuilderParams.leafRandomFactor));
            meshControls.leafsPerBranch->SetValue(defaults.meshBuilderParams.leafsPerBranch);
            meshControls.leafSpread->SetValue(wxString::Format("%.4f", defaults.meshBuilderParams.leafSpread));
            meshControls.minBranchRadius->SetValue(wxString::Format("%.6f", defaults.meshBuilderParams.minBranchRadius));
            meshControls.maxBranchRadius->SetValue(wxString::Format("%.4f", defaults.meshBuilderParams.maxBranchRadius));
            meshControls.textureAtlasSize->SetValue(defaults.meshBuilderParams.textureAtlasSize);
        });

    if (dlg.ShowModal() != wxID_OK)
        return std::nullopt;

    TreeGenerationParams out = initial;

    out.attractorsCount         = attractorsCount->GetValue();
    out.crownRadius             = wxAtof(crownRadius->GetValue());
    out.trunkMaterialTiledScale = wxAtof(tileScale->GetValue());
    out.maxDistance             = wxAtof(maxDistance->GetValue());
    out.minDistance             = wxAtof(minDistance->GetValue());
    out.segmentLength           = wxAtof(segmentLength->GetValue());
    out.crownYOffset            = wxAtof(crownYOffset->GetValue());

    out.rootPosition.x = wxAtof(rootPosition.x->GetValue());
    out.rootPosition.y = wxAtof(rootPosition.y->GetValue());
    out.rootPosition.z = wxAtof(rootPosition.z->GetValue());

    out.rootDirection.x = wxAtof(rootDirection.x->GetValue());
    out.rootDirection.y = wxAtof(rootDirection.y->GetValue());
    out.rootDirection.z = wxAtof(rootDirection.z->GetValue());

    out.trunkMaterialBaseColorTexture        = trunkAlbedo->GetPath().ToStdString();
    out.trunkMaterialAmbientOcclusionTexture = trunkAO->GetPath().ToStdString();
    out.trunkMaterialDisplacementTexture     = trunkDisp->GetPath().ToStdString();
    out.trunkMaterialMetallicTexture         = trunkMetallic->GetPath().ToStdString();
    out.trunkMaterialNormalTexture           = trunkNormal->GetPath().ToStdString();
    out.trunkMaterialRoughnessTexture        = trunkRoughness->GetPath().ToStdString();

    out.leafMaterialBaseColorTexture = leafAlbedo->GetPath().ToStdString();
    out.leafMaterialOpacityTexture   = leafOpacity->GetPath().ToStdString();
    out.leafMaterialRoughnessTexture = leafRoughness->GetPath().ToStdString();
    out.leafMaterialNormalTexture    = leafNormal->GetPath().ToStdString();

    return out;
}

}  // namespace
namespace WxEditor
{
void GenerateTree(wxFrame* parent, GLCanvas* canvas)
{
    LOG_DEBUG << "Create tree";
    auto params = EditTreeGenerationParams(parent, treeBuidlerParams);
    if (not params)
        return;

    auto dlg = std::make_shared<LoadingDialog>(parent, "Tree generator", "Generate tree...");

    std::thread(
        [dlg, parent, canvas, params]()
        {
            GameEngine::Tree tree{};
            tree.rootPosition  = vec3(0, -3.f, 0);
            tree.rootPosition  = params->rootPosition;
            tree.rootDirection = params->rootDirection;
            tree.maxDistance   = params->maxDistance;
            tree.minDistance   = params->minDistance;
            tree.segmentLength = params->segmentLength;
            tree.crownYOffset  = params->crownYOffset;

            tree.prepareAttractors(params->attractorsCount, params->crownRadius);

            auto status = tree.build();

            if (status and not status->empty())
            {
                LOG_WARN << status.value();
            }

            LOG_DEBUG << "Buildng tree mesh... ("
                      << "Branches : " << tree.GetBranches().size() << ")";
            GameEngine::TreeMeshBuilder builder(tree.GetBranches());
            auto treeMesh = builder.build(params->meshBuilderParams);
            if (treeMesh.positions_.empty())
            {
                wxLogMessage("generateTree failed");
                return;
            }

            auto& engineContext   = canvas->GetEngine().GetEngineContext();
            auto& resourceManager = canvas->GetScene().GetResourceManager();
            auto& tl              = resourceManager.GetTextureLoader();

            auto model    = std::make_unique<GameEngine::Model>();
            auto modelPtr = model.get();

            GameEngine::TextureParameters tp;
            tp.mimap = GraphicsApi::TextureMipmap::LINEAR;

            auto trunkMaterial                    = GameEngine::MaterialPresets::Trunk();
            trunkMaterial.baseColorTexture        = tl.LoadTexture(params->trunkMaterialBaseColorTexture, tp);
            trunkMaterial.ambientOcclusionTexture = tl.LoadTexture(params->trunkMaterialAmbientOcclusionTexture, tp);
            trunkMaterial.displacementTexture     = tl.LoadTexture(params->trunkMaterialDisplacementTexture, tp);
            trunkMaterial.metallicTexture         = tl.LoadTexture(params->trunkMaterialMetallicTexture, tp);
            trunkMaterial.normalTexture           = tl.LoadTexture(params->trunkMaterialNormalTexture, tp);
            trunkMaterial.roughnessTexture        = tl.LoadTexture(params->trunkMaterialRoughnessTexture, tp);
            trunkMaterial.tiledScale              = params->trunkMaterialTiledScale;

            model->AddMesh(
                GameEngine::Mesh(GraphicsApi::RenderType::TRIANGLES, engineContext.GetGraphicsApi(), treeMesh, trunkMaterial));
            resourceManager.AddModel(std::move(model));

            GameEngine::Material leafMaterial = GameEngine::MaterialPresets::Leaf();
            leafMaterial.baseColorTexture     = tl.LoadTexture(params->leafMaterialBaseColorTexture, tp);
            leafMaterial.opacityTexture       = tl.LoadTexture(params->leafMaterialOpacityTexture, tp);
            leafMaterial.roughnessTexture     = tl.LoadTexture(params->leafMaterialRoughnessTexture, tp);
            leafMaterial.normalTexture        = tl.LoadTexture(params->leafMaterialNormalTexture, tp);

            auto obj  = canvas->GetScene().CreateGameObject("GeneratedTree");
            auto& trc = obj->AddComponent<GameEngine::Components::TreeRendererComponent>();
            trc.SetGeneratedModel(modelPtr).SetLeafPosition(builder.GetLeafs()).SetLeafMaterial(leafMaterial);
            obj->SetWorldPosition(canvas->GetWorldPosFromCamera());
            canvas->AddGameObject(std::move(obj));
            parent->CallAfter([dlg]() { dlg->Close(); });
        })
        .detach();

    dlg->Show();
}
}  // namespace WxEditor
