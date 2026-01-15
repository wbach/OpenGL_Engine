#include "TreeGeneration.h"

#include <GameEngine/Components/Renderer/Trees/Leaf.h>
#include <GameEngine/Components/Renderer/Trees/Tree.h>
#include <GameEngine/Components/Renderer/Trees/TreeMeshBuilder.h>
#include <GameEngine/Components/Renderer/Trees/TreeRendererComponent.h>
#include <GameEngine/Components/Renderer/Trees/TreeUtils.h>
#include <GameEngine/Resources/ITextureLoader.h>
#include <GameEngine/Resources/Models/Material.h>
#include <GameEngine/Resources/Models/MaterialPresets.h>
#include <GameEngine/Resources/Models/ModelWrapper.h>
#include <GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h>
#include <GameEngine/Shaders/ShaderProgram.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>
#include <wx/wx.h>

#include <GameEngine/Resources/IResourceManager.hpp>
#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/MeshRawData.h"
#include "Image/Image.h"
#include "Image/ImageUtils.h"
#include "LinearMath/btAlignedAllocator.h"
#include "Logger/Log.h"
#include "Resources/DataStorePolicy.h"
#include "Resources/Models/Mesh.h"
#include "Resources/Models/Model.h"
#include "WxEditor/WxHelpers/LoadingDialog.h"

namespace
{
struct TreeGenerationParams
{
    size_t attractorsCount        = 400;
    vec3 crownSize                = vec3{10.f, 6.f, 8.f};
    float trunkMaterialTiledScale = 2.f;
    float maxDistance{5.f};
    float minDistance{1.f};
    float segmentLength{0.3f};
    float crownYOffset{3.f};
    vec3 rootPosition{0.f};
    vec3 rootDirection{0.f, 1.f, 0.f};
    std::string trunkMaterialBaseColorTexture        = "Textures/Tree/trunk/light-tree-bark_albedo.png";
    std::string trunkMaterialAmbientOcclusionTexture = "Textures/Tree/trunk/light-tree-bark_ao.png";
    std::string trunkMaterialDisplacementTexture     = "Textures/Tree/trunk/light-tree-bark_albedo.png";
    std::string trunkMaterialMetallicTexture         = "Textures/Tree/trunk/light-tree-bark_metallic.png";
    std::string trunkMaterialNormalTexture           = "Textures/Tree/trunk/light-tree-bark_normal-ogl.png";
    std::string trunkMaterialRoughnessTexture        = "Textures/Tree/trunk/light-tree-bark_roughness.png";
    std::string leafMaterialBaseColorTexture         = "Textures/Tree/Leafs/LeafSet024_2K-PNG_ColorWB.png";
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

    // --- FlexGridSizer 3 kolumny: label + ctrl * 3 ---
    auto* grid = new wxFlexGridSizer(0, 6, 8, 8);
    grid->AddGrowableCol(1, 1);
    grid->AddGrowableCol(3, 1);
    grid->AddGrowableCol(5, 1);

    // --- Helpery ---
    auto addFloat = [&](const wxString& label, float value)
    {
        grid->Add(new wxStaticText(&dlg, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);
        auto* ctrl = new wxTextCtrl(&dlg, wxID_ANY, wxString::Format("%.4f", value), wxDefaultPosition, wxSize(120, -1));
        grid->Add(ctrl, 1, wxEXPAND);
        return ctrl;
    };

    auto addSizeT = [&](const wxString& label, size_t value)
    {
        grid->Add(new wxStaticText(&dlg, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);
        auto* ctrl = new wxSpinCtrl(&dlg, wxID_ANY);
        ctrl->SetRange(0, 1'000'000);
        ctrl->SetValue(static_cast<int>(value));
        ctrl->SetSize(wxSize(120, -1));
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
    auto crownSize        = addVec3("Crown size", initial.crownSize);
    auto* tileScale       = addFloat("Trunk Tiled Scale", initial.trunkMaterialTiledScale);
    auto* maxDistance     = addFloat("SCA Max distance", initial.maxDistance);
    auto* minDistance     = addFloat("SCA Min distance", initial.minDistance);
    auto* segmentLength   = addFloat("Trunk segment scale", initial.segmentLength);
    auto* crownYOffset    = addFloat("Crown y offset", initial.crownYOffset);

    auto rootPosition  = addVec3("Root position", initial.rootPosition);
    auto rootDirection = addVec3("Root direction", initial.rootDirection);

    // --- EntryParameters ---
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

    // --- Buttons: Restore / OK / Cancel ---
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

    // --- Restore Defaults ---
    restoreBtn->Bind(
        wxEVT_BUTTON,
        [&](wxCommandEvent&)
        {
            // Basic
            attractorsCount->SetValue(static_cast<int>(defaults.attractorsCount));

            tileScale->SetValue(wxString::Format("%.4f", defaults.trunkMaterialTiledScale));
            maxDistance->SetValue(wxString::Format("%.4f", defaults.maxDistance));
            minDistance->SetValue(wxString::Format("%.4f", defaults.minDistance));
            segmentLength->SetValue(wxString::Format("%.4f", defaults.segmentLength));
            crownYOffset->SetValue(wxString::Format("%.4f", defaults.crownYOffset));

            // Vec3
            crownSize.x->SetValue(wxString::Format("%.4f", defaults.crownSize.x));
            crownSize.y->SetValue(wxString::Format("%.4f", defaults.crownSize.y));
            crownSize.z->SetValue(wxString::Format("%.4f", defaults.crownSize.z));

            rootPosition.x->SetValue(wxString::Format("%.4f", defaults.rootPosition.x));
            rootPosition.y->SetValue(wxString::Format("%.4f", defaults.rootPosition.y));
            rootPosition.z->SetValue(wxString::Format("%.4f", defaults.rootPosition.z));

            rootDirection.x->SetValue(wxString::Format("%.4f", defaults.rootDirection.x));
            rootDirection.y->SetValue(wxString::Format("%.4f", defaults.rootDirection.y));
            rootDirection.z->SetValue(wxString::Format("%.4f", defaults.rootDirection.z));

            // Trunk textures
            trunkAlbedo->SetPath(wxString::FromUTF8(defaults.trunkMaterialBaseColorTexture.c_str()));
            trunkAO->SetPath(wxString::FromUTF8(defaults.trunkMaterialAmbientOcclusionTexture.c_str()));
            trunkDisp->SetPath(wxString::FromUTF8(defaults.trunkMaterialDisplacementTexture.c_str()));
            trunkMetallic->SetPath(wxString::FromUTF8(defaults.trunkMaterialMetallicTexture.c_str()));
            trunkNormal->SetPath(wxString::FromUTF8(defaults.trunkMaterialNormalTexture.c_str()));
            trunkRoughness->SetPath(wxString::FromUTF8(defaults.trunkMaterialRoughnessTexture.c_str()));

            // Leaf textures
            leafAlbedo->SetPath(wxString::FromUTF8(defaults.leafMaterialBaseColorTexture.c_str()));
            leafOpacity->SetPath(wxString::FromUTF8(defaults.leafMaterialOpacityTexture.c_str()));
            leafRoughness->SetPath(wxString::FromUTF8(defaults.leafMaterialRoughnessTexture.c_str()));
            leafNormal->SetPath(wxString::FromUTF8(defaults.leafMaterialNormalTexture.c_str()));

            // EntryParameters
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

    // --- Odczyt wyników ---
    TreeGenerationParams out = initial;

    out.attractorsCount = attractorsCount->GetValue();

    out.crownSize.x = wxAtof(crownSize.x->GetValue());
    out.crownSize.y = wxAtof(crownSize.y->GetValue());
    out.crownSize.z = wxAtof(crownSize.z->GetValue());

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

    out.meshBuilderParams.radialSegments     = meshControls.radialSegments->GetValue();
    out.meshBuilderParams.leafheightTreshold = wxAtof(meshControls.leafheightThreshold->GetValue());
    out.meshBuilderParams.leafRandomFactor   = wxAtof(meshControls.leafRandomFactor->GetValue());
    out.meshBuilderParams.leafsPerBranch     = meshControls.leafsPerBranch->GetValue();
    out.meshBuilderParams.leafSpread         = wxAtof(meshControls.leafSpread->GetValue());
    out.meshBuilderParams.minBranchRadius    = wxAtof(meshControls.minBranchRadius->GetValue());
    out.meshBuilderParams.maxBranchRadius    = wxAtof(meshControls.maxBranchRadius->GetValue());
    out.meshBuilderParams.textureAtlasSize   = meshControls.textureAtlasSize->GetValue();

    return out;
}

}  // namespace

namespace WxEditor
{
struct TreeModel
{
    GameEngine::Model* trunkModel;
    GameEngine::Model* leafBilboardModel;

    std::vector<GameEngine::Leaf> leafs;
    GameEngine::Material leafMaterial;
};

void DrawLeafShape(GameEngine::Billboard& bb, int ix, int iy, int radius, const GameEngine::Leaf& leaf)
{
    float dirU = glm::dot(leaf.direction, bb.tangent);
    float dirV = glm::dot(leaf.direction, bb.bitangent);

    float angle = atan2(dirV, dirU);
    float cosA  = cos(angle);
    float sinA  = sin(angle);

    float lengthOnPlane = sqrt(dirU * dirU + dirV * dirV);
    float elongation    = 1.0f + lengthOnPlane * 2.5f;
    int resolution      = bb.resolution;

    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            float rx = (dx * cosA + dy * sinA) / elongation;
            float ry = (-dx * sinA + dy * cosA);

            if (rx * rx + ry * ry > radius * radius)
                continue;

            int px = ix + dx;
            int py = iy + dy;

            if (px >= 0 && py >= 0 && px < resolution && py < resolution)
            {
                int idx           = py * resolution + px;
                float dist        = sqrt(rx * rx + ry * ry);
                float alphaFactor = 1.0f - (dist / float(radius));

                bb.alpha[idx] += 0.22f * leaf.sizeRandomness * alphaFactor;
            }
        }
    }
}

void DrawCircleShape(GameEngine::Billboard& bb, int ix, int iy, int radius, const GameEngine::Leaf& leaf)
{
    int resolution = bb.resolution;

    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            if (dx * dx + dy * dy > radius * radius)
                continue;

            int px = ix + dx;
            int py = iy + dy;

            if (px >= 0 && py >= 0 && px < resolution && py < resolution)
            {
                int idx           = py * resolution + px;
                float dist        = sqrt(float(dx * dx + dy * dy));
                float alphaFactor = 1.0f - (dist / float(radius));

                bb.alpha[idx] += 0.15f * leaf.sizeRandomness * alphaFactor;
            }
        }
    }
}

glm::vec3 CalculateLeavesCentroid(const std::vector<GameEngine::Leaf>& leaves)
{
    if (leaves.empty())
        return glm::vec3(0.0f);

    glm::vec3 center(0.0f);
    for (const auto& l : leaves)
    {
        center += l.position;
    }

    return center / static_cast<float>(leaves.size());
}

std::vector<glm::vec3> GetBillboardNormals()
{
    return {
        glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)),
        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)), glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f)),
        glm::vec3(0.0f, -1.0f, 0.0f)  // top
    };
}

void RasterizeLeavesToBillboard(GameEngine::Billboard& bb, const std::vector<GameEngine::Leaf>& leaves, const glm::vec3& center,
                                int resolution)
{
    for (const auto& l : leaves)
    {
        auto d = l.position - center;
        auto u = glm::dot(d, bb.tangent);
        auto v = glm::dot(d, bb.bitangent);

        auto x = (u / bb.extentU * 0.5f + 0.5f) * (resolution - 1);
        auto y = (v / bb.extentV * 0.5f + 0.5f) * (resolution - 1);

        auto ix = static_cast<int>(x);
        auto iy = static_cast<int>(y);
        if (ix < 0 || iy < 0 || ix >= resolution || iy >= resolution)
            continue;

        auto radius = std::max(int(l.sizeRandomness * resolution * 0.015f), 1);

        DrawLeafShape(bb, ix, iy, radius, l);
        // DrawCircleShape(bb, ix, iy, radius, l);
    }
}

std::vector<GameEngine::Billboard> GenerateTreeBillboards(const std::vector<GameEngine::Leaf>& leaves, int resolution = 128)
{
    vec3 center  = CalculateLeavesCentroid(leaves);
    auto normals = GetBillboardNormals();

    std::vector<GameEngine::Billboard> result;

    for (const auto& normal : normals)
    {
        GameEngine::Billboard bb;
        bb.resolution = resolution;
        bb.alpha.assign(resolution * resolution, 0.0f);

        bb.center = center;
        bb.normal = normalize(normal);

        // lokalna baza dla billboardu
        vec3 up      = abs(bb.normal.y) < 0.99f ? vec3(0, 1, 0) : vec3(1, 0, 0);
        bb.tangent   = normalize(cross(up, bb.normal));
        bb.bitangent = cross(bb.normal, bb.tangent);

        // wylicz extentU/V
        bb.extentU = 0.0f;
        bb.extentV = 0.0f;
        for (const auto& l : leaves)
        {
            vec3 d  = l.position - center;
            float u = dot(d, bb.tangent);
            float v = dot(d, bb.bitangent);

            bb.extentU = std::max(bb.extentU, abs(u));
            bb.extentV = std::max(bb.extentV, abs(v));
        }

        bb.extentU = std::max(bb.extentU, 0.001f);
        bb.extentV = std::max(bb.extentV, 0.001f);

        RasterizeLeavesToBillboard(bb, leaves, center, resolution);

        // clamp do [0,1]
        for (float& a : bb.alpha)
            a = glm::clamp(a, 0.0f, 1.0f);

        result.push_back(bb);
    }

    return result;
}

std::unique_ptr<GameEngine::Model> CreateBillboardVertices(GraphicsApi::IGraphicsApi& graphicsApi,
                                                           GameEngine::ITextureLoader& textureLoader,
                                                           const std::vector<GameEngine::Billboard>& billboards)
{
    auto model = std::make_unique<GameEngine::Model>();

    for (size_t i = 0; i < billboards.size(); ++i)
    {
        GraphicsApi::MeshRawData meshData;
        const auto& b = billboards[i];

        glm::vec3 p0 = b.center - b.tangent * b.extentU - b.bitangent * b.extentV;
        glm::vec3 p1 = b.center + b.tangent * b.extentU - b.bitangent * b.extentV;
        glm::vec3 p2 = b.center + b.tangent * b.extentU + b.bitangent * b.extentV;
        glm::vec3 p3 = b.center - b.tangent * b.extentU + b.bitangent * b.extentV;

        // Trójkąt 1
        meshData.positions_.push_back(p0.x);
        meshData.positions_.push_back(p0.y);
        meshData.positions_.push_back(p0.z);

        meshData.textCoords_.push_back(0);
        meshData.textCoords_.push_back(0);

        meshData.normals_.push_back(i);
        meshData.normals_.push_back(0);
        meshData.normals_.push_back(0);

        meshData.positions_.push_back(p1.x);
        meshData.positions_.push_back(p1.y);
        meshData.positions_.push_back(p1.z);

        meshData.textCoords_.push_back(1);
        meshData.textCoords_.push_back(0);

        meshData.normals_.push_back(i);
        meshData.normals_.push_back(0);
        meshData.normals_.push_back(0);

        meshData.positions_.push_back(p2.x);
        meshData.positions_.push_back(p2.y);
        meshData.positions_.push_back(p2.z);

        meshData.textCoords_.push_back(1);
        meshData.textCoords_.push_back(1);

        meshData.normals_.push_back(i);
        meshData.normals_.push_back(0);
        meshData.normals_.push_back(0);

        // Trójkąt 2
        meshData.positions_.push_back(p0.x);
        meshData.positions_.push_back(p0.y);
        meshData.positions_.push_back(p0.z);

        meshData.textCoords_.push_back(0);
        meshData.textCoords_.push_back(0);

        meshData.normals_.push_back(i);
        meshData.normals_.push_back(0);
        meshData.normals_.push_back(0);

        meshData.positions_.push_back(p2.x);
        meshData.positions_.push_back(p2.y);
        meshData.positions_.push_back(p2.z);

        meshData.textCoords_.push_back(1);
        meshData.textCoords_.push_back(1);

        meshData.normals_.push_back(i);
        meshData.normals_.push_back(0);
        meshData.normals_.push_back(0);

        meshData.positions_.push_back(p3.x);
        meshData.positions_.push_back(p3.y);
        meshData.positions_.push_back(p3.z);

        meshData.textCoords_.push_back(0);
        meshData.textCoords_.push_back(1);

        meshData.normals_.push_back(i);
        meshData.normals_.push_back(0);
        meshData.normals_.push_back(0);

        Utils::Image image;
        image.width  = b.resolution;
        image.height = b.resolution;
        image.setChannels(1);
        image.moveData(std::move(b.alpha));

        GameEngine::Material material = GameEngine::MaterialPresets::Leaf();
        material.baseColorTexture     = material.diffuseTexture =
            textureLoader.CreateTexture(Utils::CreateUniqueFilename(),
                                        GameEngine::TextureParameters{.dataStorePolicy = GameEngine::DataStorePolicy::Store,
                                                                      .filter          = GraphicsApi::TextureFilter::LINEAR},
                                        std::move(image));

        GameEngine::Mesh mesh(GraphicsApi::RenderType::TRIANGLES, graphicsApi, std::move(meshData), material);
        model->AddMesh(std::move(mesh));
    }
    return model;
}

void GenerateBibloardImage(const GameEngine::TreeMeshBuilder& tree)
{
    int resolution{512};
    auto bilboards = GenerateTreeBillboards(tree.GetLeafs(), resolution);

    int i = 0;
    for (auto& b : bilboards)
    {
        Utils::Image image;
        image.width  = resolution;
        image.height = resolution;
        image.setChannels(1);
        image.moveData(std::move(b.alpha));
        Utils::SaveImage(image, "./LeafsBilboard_" + std::to_string(i++) + ".png");
    }
}

GameEngine::TreeClusters groupLeavesIntoClusters(const std::vector<GameEngine::Leaf>& leaves, float clusterSize)
{
    if (leaves.empty())
        return {};

    vec3 minBound = leaves[0].position;
    vec3 maxBound = leaves[0].position;

    for (const auto& leaf : leaves)
    {
        minBound = min(minBound, leaf.position);
        maxBound = max(maxBound, leaf.position);
    }

    minBound -= 0.01f;
    maxBound += 0.01f;

    vec3 diagonal  = maxBound - minBound;
    vec3i gridSize = vec3i(ceil(diagonal.x / clusterSize), ceil(diagonal.y / clusterSize), ceil(diagonal.z / clusterSize));

    GameEngine::TreeClusters result;
    result.gridOrigin = minBound;
    result.voxelSize  = vec3(clusterSize);
    result.gridSize   = gridSize;

    std::unordered_map<int, GameEngine::Cluster> activeClusters;

    for (size_t i = 0; i < leaves.size(); ++i)
    {
        vec3i coord = vec3i((leaves[i].position - minBound) / clusterSize);
        int key = coord.x + coord.y * gridSize.x + coord.z * gridSize.x * gridSize.y;

        if (activeClusters.find(key) == activeClusters.end())
        {
            GameEngine::Cluster newCluster;
            newCluster.minBound = minBound + vec3(coord) * clusterSize;
            newCluster.maxBound = newCluster.minBound + vec3(clusterSize);
            activeClusters[key] = newCluster;
        }

        activeClusters[key].leafIndices.push_back(i);
    }

    for (auto const& [key, cluster] : activeClusters)
    {
        result.clusters.push_back(cluster);
    }

    return result;
}

GameEngine::Tree GenerateTree(const TreeGenerationParams& params)
{
    GameEngine::Tree tree{};
    tree.rootPosition  = params.rootPosition;
    tree.rootDirection = params.rootDirection;
    tree.maxDistance   = params.maxDistance;
    tree.minDistance   = params.minDistance;
    tree.segmentLength = params.segmentLength;
    tree.crownYOffset  = params.crownYOffset;

    // spehere
    // tree.prepareAttractors(params.attractorsCount, params.crownRadius);

    // elips
    auto noiseStrength = 0.05f - 0.15f * params.crownSize.y;
    tree.prepareAttractors(params.attractorsCount, params.crownSize, noiseStrength);

    auto status = tree.build();

    if (status and not status->empty())
    {
        LOG_WARN << status.value();
    }

    return tree;
}

std::pair<GameEngine::Material, GameEngine::Material> PrepareTreeMaterials(GameEngine::ITextureLoader& tl,
                                                                           const TreeGenerationParams& params)
{
    GameEngine::TextureParameters tp;
    tp.mimap = GraphicsApi::TextureMipmap::LINEAR;

    auto trunkMaterial                    = GameEngine::MaterialPresets::Trunk();
    trunkMaterial.baseColorTexture        = tl.LoadTexture(params.trunkMaterialBaseColorTexture, tp);
    trunkMaterial.ambientOcclusionTexture = tl.LoadTexture(params.trunkMaterialAmbientOcclusionTexture, tp);
    trunkMaterial.displacementTexture     = tl.LoadTexture(params.trunkMaterialDisplacementTexture, tp);
    trunkMaterial.metallicTexture         = tl.LoadTexture(params.trunkMaterialMetallicTexture, tp);
    trunkMaterial.normalTexture           = tl.LoadTexture(params.trunkMaterialNormalTexture, tp);
    trunkMaterial.roughnessTexture        = tl.LoadTexture(params.trunkMaterialRoughnessTexture, tp);
    trunkMaterial.tiledScale              = params.trunkMaterialTiledScale;

    GameEngine::Material leafMaterial = GameEngine::MaterialPresets::Leaf();
    leafMaterial.baseColorTexture     = tl.LoadTexture(params.leafMaterialBaseColorTexture, tp);
    leafMaterial.opacityTexture       = tl.LoadTexture(params.leafMaterialOpacityTexture, tp);
    leafMaterial.roughnessTexture     = tl.LoadTexture(params.leafMaterialRoughnessTexture, tp);
    leafMaterial.normalTexture        = tl.LoadTexture(params.leafMaterialNormalTexture, tp);

    return {trunkMaterial, leafMaterial};
}

std::optional<TreeModel> GenerateLoD1Tree(const GameEngine::Tree& tree, GLCanvas* canvas, const TreeGenerationParams& params)
{
    LOG_DEBUG << "Buildng tree mesh lod 1 ... ("
              << "Branches : " << tree.GetBranches().size() << ")";
    GameEngine::TreeMeshBuilder builder(tree.GetBranches());

    auto treeMesh = builder.build(params.meshBuilderParams);
    if (treeMesh.positions_.empty())
    {
        wxLogMessage("generateTree failed");
        return std::nullopt;
    }

    auto& engineContext   = canvas->GetEngine().GetEngineContext();
    auto& resourceManager = canvas->GetScene().GetResourceManager();

    auto trunkModel = std::make_unique<GameEngine::Model>();

    const auto [trunkMaterial, leafMaterial] = PrepareTreeMaterials(resourceManager.GetTextureLoader(), params);

    trunkModel->AddMesh(
        GameEngine::Mesh(GraphicsApi::RenderType::TRIANGLES, engineContext.GetGraphicsApi(), std::move(treeMesh), trunkMaterial));
    auto trunkModelPtr = trunkModel.get();
    resourceManager.AddModel(std::move(trunkModel));

    auto leafBilboardModel    = CreateBillboardVertices(engineContext.GetGraphicsApi(), resourceManager.GetTextureLoader(),
                                                        GenerateTreeBillboards(builder.GetLeafs(), 512));
    auto leafBilboardModelPtr = leafBilboardModel.get();
    resourceManager.AddModel(std::move(leafBilboardModel));

    return TreeModel{.trunkModel        = trunkModelPtr,
                     .leafBilboardModel = leafBilboardModelPtr,
                     .leafs             = builder.GetLeafs(),
                     .leafMaterial      = leafMaterial};
}

void BindMaterialTexture(GraphicsApi::IGraphicsApi& graphicsApi, uint32 location, GameEngine::GeneralTexture* texture,
                         bool enabled)
{
    if (enabled and texture and texture->GetGraphicsObjectId())
    {
        graphicsApi.ActiveTexture(location, *texture->GetGraphicsObjectId());
    }
}

void BindMaterial(GraphicsApi::IGraphicsApi& graphicsApi, const GameEngine::Material& material)
{
    if (material.flags & MAT_DOUBLE_SIDED || (material.flags & MAT_FOLIAGE))
    {
        graphicsApi.DisableCulling();
    }
    else
    {
        graphicsApi.EnableCulling();
    }

    const auto& config = EngineConf.renderer.textures;

    BindMaterialTexture(graphicsApi, 0, material.baseColorTexture, config.useDiffuse);
    BindMaterialTexture(graphicsApi, 1, material.normalTexture, config.useNormal);
    BindMaterialTexture(graphicsApi, 2, material.roughnessTexture, config.useRoughness);
    BindMaterialTexture(graphicsApi, 3, material.metallicTexture, config.useMetallic);
    BindMaterialTexture(graphicsApi, 4, material.ambientOcclusionTexture, config.useAmientOcclusion);
    BindMaterialTexture(graphicsApi, 5, material.opacityTexture, config.useOpacity);
    BindMaterialTexture(graphicsApi, 6, material.displacementTexture, config.useDisplacement);
}

std::optional<TreeModel> GenerateLoD2Tree(const GameEngine::Tree& tree, GLCanvas* canvas, const TreeGenerationParams& params)
{
    LOG_DEBUG << "Buildng tree mesh lod 2... ("
              << "Branches : " << tree.GetBranches().size() << ")";
    GameEngine::TreeMeshBuilder builder(tree.GetBranches());
    auto lod2Params           = params.meshBuilderParams;
    lod2Params.radialSegments = 3;

    auto treeMesh = builder.build(lod2Params);
    if (treeMesh.positions_.empty())
    {
        wxLogMessage("generateTree failed");
        return std::nullopt;
    }

    auto& engineContext   = canvas->GetEngine().GetEngineContext();
    auto& resourceManager = canvas->GetScene().GetResourceManager();

    auto trunkModel = std::make_unique<GameEngine::Model>();

    const auto [trunkMaterial, leafMaterial] = PrepareTreeMaterials(resourceManager.GetTextureLoader(), params);

    trunkModel->AddMesh(
        GameEngine::Mesh(GraphicsApi::RenderType::TRIANGLES, engineContext.GetGraphicsApi(), std::move(treeMesh), trunkMaterial));
    auto trunkModelPtr = trunkModel.get();
    resourceManager.AddModel(std::move(trunkModel));

    return TreeModel{.trunkModel = trunkModelPtr, .leafs = builder.GetLeafs(), .leafMaterial = leafMaterial};
}

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
            auto obj  = canvas->GetScene().CreateGameObject("GeneratedTree");
            auto& trc = obj->AddComponent<GameEngine::Components::TreeRendererComponent>();

            auto tree      = GenerateTree(*params);
            auto treeModel = GenerateLoD1Tree(tree, canvas, *params);

            trc.SetLeafBilboardsModel(treeModel->leafBilboardModel);
            trc.SetGeneratedTrunkModel(treeModel->trunkModel, GameEngine::LevelOfDetail::L1);
            trc.UpdateLeafsSsbo(GameEngine::PrepareSSBOData(treeModel->leafs));
            trc.SetLeafMaterial(treeModel->leafMaterial);

            auto treeModelLod2 = GenerateLoD2Tree(tree, canvas, *params);
            trc.SetGeneratedTrunkModel(treeModelLod2->trunkModel, GameEngine::LevelOfDetail::L2);

            obj->SetWorldPosition(canvas->GetWorldPosFromCamera());
            canvas->AddGameObject(std::move(obj));
            parent->CallAfter([dlg]() { dlg->Close(); });
        })
        .detach();

    dlg->Show();
}
}  // namespace WxEditor
