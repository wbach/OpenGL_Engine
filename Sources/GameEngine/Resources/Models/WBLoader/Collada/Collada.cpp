#include "Collada.h"

#include <algorithm>
#include <numeric>

#include "GameEngine/Resources/ITextureLoader.h"
#include "Logger/Log.h"
#include "Utils/GLM/GLMUtils.h"
#include "Xml/MainReader.h"

namespace GameEngine
{
static Collada::Node staticNode;
static mat4 correction_matrix = Utils::CreateTransformationMatrix(vec3(0), DegreesVec3(-90, 0, 0), vec3(1));

namespace WBLoader
{
ColladaDae::ColladaDae(ITextureLoader& textureLodaer)
    : AbstractLoader(textureLodaer.GetGraphicsApi(), textureLodaer)
{
}
void ColladaDae::ParseFile(const File& filename)
{
    Collada::ReadCollada(filename, data_);
    ConstructModel();
    FillAnimationData();
    Clear();
}
bool ColladaDae::CheckExtension(const File& file)
{
    return file.IsExtension("dae");
}
void ColladaDae::ConstructModel()
{
    for (const auto& geometry : data_.libraryGeometries_.geometries_)
    {
        NewGeometry(geometry.second);
    }
}

std::string operator+(const std::string& str, float t)
{
    return str + " " + std::to_string(t);
}

void ColladaDae::FillAnimationData()
{
    std::vector<float> boneWeightsTmp;

    for (const auto& vs : data_.libraryVisualScenes_.visualScenes_)
    {
        for (const auto& node : vs.second.nodes_)
        {
            if (GetType(node.second.name_) != Collada::ObjectType::LIBRARY_GOMETRIES && node.second.instanceController_)
            {
                if (idToMeshMap_.count(node.second.name_) == 0)
                    continue;

                auto mesh = idToMeshMap_[node.second.name_];

                const auto& ic  = node.second.instanceController_.constValue();
                auto skinSource = Collada::GetSource(ic.url_);
                auto& skin      = data_.libraryControllers_.controllers_[skinSource].skins_.begin()->second;

                for (const auto& in : skin.vertexWeights_.inputs_)
                {
                    if (in.semantic == "WEIGHT")
                    {
                        auto source = Collada::GetSource(in.sourceId);
                        Collada::GetFloatsFromString(skin.sources_[source].dataArray.data, boneWeightsTmp);
                    }
                }

                uint32 i    = 0;
                uint32 vbId = 0;
                for (auto vc : skin.vertexWeights_.vcount_)
                {
                    std::vector<JointInfo> joints;
                    for (uint32 x = 0; x < vc; ++x)
                    {
                        uint32 sumOffsets = 0;

                        joints.push_back({0, 0});
                        for (const auto& in : skin.vertexWeights_.inputs_)
                        {
                            auto offset = in.offset;
                            sumOffsets += offset;
                            auto value = skin.vertexWeights_.v_[i + offset];

                            if (in.semantic == "JOINT")
                            {
                                joints.back().id = value;
                            }
                            if (in.semantic == "WEIGHT")
                            {
                                auto weightValue     = boneWeightsTmp[value];
                                joints.back().weight = weightValue;
                            }
                        }
                        i += skin.vertexWeights_.inputs_.size();
                    }

                    std::sort(joints.begin(), joints.end(),
                              [](const JointInfo& a, const JointInfo& b) { return a.weight > b.weight; });

                    for (const auto& ii : mesh->vertexPlacesInVertexBuffer_[vbId])
                    {
                        mesh->vertexBuffer[ii].jointInfo = joints;
                    }

                    ++vbId;
                }
                const auto& skeleton = node.second.instanceController_.constValue().skeleton_;
                const auto& node     = GetNode(Collada::GetSource(skeleton));

                std::unordered_map<std::string, uint32> joints;
                for (const auto& input : skin.joitns_.inputs_)
                {
                    if (input.semantic == "JOINT")
                    {
                        const auto& data = skin.sources_[Collada::GetSource(input.sourceId)].dataArray;
                        if (data.type == "Name_array")
                        {
                            joints = Utils::SplitStringWithId(data.data, ' ');
                        }
                        break;
                    }
                }
                CreateSkeleton(node, mesh->skeleton_, joints);
                FillAnimator(mesh->animationClips_, joints);
            }
        }
    }
}
void ColladaDae::NewGeometry(const Collada::Geometry& geomtery)
{
    objects.emplace_back();
    objects.back().transformMatrix = correction_matrix;
    for (const auto& mesh : geomtery.meshes_)
    {
        NewMesh(mesh, geomtery.name_);
    }
}
void ColladaDae::NewMesh(const Collada::Mesh& mesh, const std::string& geometryName)
{
    for (const auto& pair : mesh.polylist_)
    {
        auto& polyList = pair.second;

        objects.back().meshes.emplace_back();
        auto& newMesh   = objects.back().meshes.back();
        auto meshSource = Collada::GetSource(polyList.materialId_);
        ApplyMaterials(newMesh.material, meshSource);
        idToMeshMap_[geometryName] = &newMesh;

        for (const auto& input : polyList.inputs_)
        {
            PrepareMeshData(mesh, newMesh, input);
        }

        uint32 i    = 0;
        uint32 vbId = 0;
        for (auto vc : polyList.vcount_)
        {
            for (uint32 x = 0; x < vc; ++x)
            {
                WBLoader::VertexBuffer vb;
                uint32 sumOffsets = 0;
                for (const auto& input : polyList.inputs_)
                {
                    auto offset = input.offset;
                    sumOffsets += offset;
                    auto value = polyList.p_[i + offset];

                    if (input.semantic == "VERTEX")
                    {
                        vb.position  = newMesh.vertex[value];
                        vb.indexes.x = value;
                        newMesh.vertexPlacesInVertexBuffer_[value].push_back(vbId);
                    }
                    if (input.semantic == "NORMAL")
                    {
                        vb.normal    = newMesh.normals[value];
                        vb.indexes.y = value;
                    }
                    if (input.semantic == "TEXCOORD")
                    {
                        vb.uvs       = newMesh.text_coords[value];
                        vb.indexes.z = value;
                    }
                    if (input.semantic == "COLOR")
                    {
                    }
                }
                newMesh.vertexBuffer.push_back(vb);
                ++vbId;
                i += polyList.inputs_.size();
            }
        }
    }
}
void ColladaDae::GetVec2ArrayFromDataSource(const Collada::Mesh& mesh, const std::string& sourceId,
                                            std::vector<vec2>& v)
{
    if (mesh.sources_.count(sourceId) == 0)
        return;

    const auto& datasource = mesh.sources_.at(sourceId);
    const auto& params     = datasource.technique.accessor.params;

    if (params.empty())
        return;

    v.reserve(datasource.dataArray.count);
    Collada::GetVectors2dFromString(datasource.dataArray.data, v);
}
void ColladaDae::GetVec3ArrayFromDataSource(const Collada::Mesh& mesh, const std::string& sourceId,
                                            std::vector<vec3>& v)
{
    if (mesh.sources_.count(sourceId) == 0)
        return;

    const auto& datasource = mesh.sources_.at(sourceId);
    const auto& params     = datasource.technique.accessor.params;

    if (params.empty())
        return;

    v.reserve(datasource.dataArray.count);
    Collada::GetVectors3dFromString(datasource.dataArray.data, v);
}
void ColladaDae::PrepareMeshData(const Collada::Mesh& colladaMesh, Mesh& newMesh, const Collada::Input& input)
{
    auto source = Collada::GetSource(input.sourceId);

    if (input.semantic == "VERTEX")
    {
        if (colladaMesh.vertices_.count(source) != 0)
        {
            for (const auto& vertInput : colladaMesh.vertices_.at(source).inputs_)
            {
                if (vertInput.semantic == "POSITION")
                {
                    auto vertSourceId = Collada::GetSource(vertInput.sourceId);
                    if (colladaMesh.sources_.count(vertSourceId) != 0)
                    {
                        GetVec3ArrayFromDataSource(colladaMesh, vertSourceId, newMesh.vertex);
                    }
                    else
                    {
                        ERROR_LOG("POSITION : Undefined reference to : " + vertInput.sourceId);
                    }
                }
            }
        }
        else
        {
            GetVec3ArrayFromDataSource(colladaMesh, source, newMesh.vertex);
        }
    }
    else if (input.semantic == "NORMAL")
    {
        GetVec3ArrayFromDataSource(colladaMesh, source, newMesh.normals);
    }
    else if (input.semantic == "TEXCOORD")
    {
        GetVec2ArrayFromDataSource(colladaMesh, source, newMesh.text_coords);
    }
    else if (input.semantic == "COLOR")
    {
    }
}
Collada::DataTypes ColladaDae::GetSourceType(const Collada::Source& source) const
{
    const auto& params = source.technique.accessor.params;

    if (params.empty())
        return Collada::DataTypes();

    // expect all as floats param
    if (params.front().type == "float" && params.size() == 3)
        return Collada::DataTypes::VEC3;

    if (params.front().type == "float" && params.size() == 2)
        return Collada::DataTypes::VEC2;

    return Collada::DataTypes();
}

void ConvertJoint(const Collada::Node& node, Animation::Joint& joint)
{
    joint.name = node.name_;
    if (node.matrix_)
    {
        joint.transform = node.matrix_.constValue().matrix_;
        // joint.invTransform = glm::inverse(joint.transform);
    }
}
void ColladaDae::CreateSkeleton(const Collada::Node& node, Animation::Joint& joint,
                                const std::unordered_map<std::string, uint32>& joints)
{
    if (joints.count(node.sid_) != 0)
        joint.id = joints.at(node.sid_);

    joint.name = node.sid_;
    if (node.matrix_)
    {
        joint.transform = node.matrix_.constValue().matrix_;
    }

    joint.size = 1;

    for (const auto& pair : node.children_)
    {
        if (pair.second == nullptr)
            continue;

        auto& child = *pair.second;
        joint.children.emplace_back();
        CreateSkeleton(child, joint.children.back(), joints);
        joint.size += joint.children.back().size;
    }
}

bool ContainJoint(const std::string& str1, const std::string& str2)
{
    for (uint16 x = 0; x < str1.size(); ++x)
    {
        uint16 y = x;
        bool is  = true;
        for (auto c2 : str2)
        {
            if (y >= str1.size())
                return false;

            if (str1[y++] != c2)
            {
                is = false;
                break;
            }
        }
        if (is)
            return true;
    }
    return false;
}

void ColladaDae::FillAnimator(std::unordered_map<std::string, Animation::AnimationClip>& animationClips,
                              const std::unordered_map<std::string, uint32>& joints)
{
    for (const auto& animationClipPair : data_.libraryAnimations_.animationsClips_)
    {
        bool isSetTimeStamp = false;
        std::vector<Animation::KeyFrame> frames;
        for (const auto& anim_pair : animationClipPair.second.animations_)
        {
            auto& anim     = anim_pair.second;
            auto& animName = anim_pair.first;

            for (const auto& joint : joints)
            {
                if (ContainJoint(animName, joint.first))
                {
                    for (const auto& input : anim.sampler_.inputs_)
                    {
                        if (input.semantic == "INPUT" && !isSetTimeStamp)
                        {
                            auto sourceId = Collada::GetSource(input.sourceId);
                            if (anim.sources_.count(sourceId) != 0)
                            {
                                auto& source   = anim.sources_.at(sourceId);
                                auto floatData = Collada::GetFloatsFromString(source.dataArray.data);

                                uint16 frameId = 0;
                                for (auto frameTime : floatData)
                                {
                                    if (frames.size() <= frameId)
                                        frames.emplace_back();
                                    frames[frameId++].timeStamp = frameTime;
                                }
                                isSetTimeStamp = true;
                            }
                        }
                        else if (input.semantic == "OUTPUT")
                        {
                            auto sourceId = Collada::GetSource(input.sourceId);
                            if (anim.sources_.count(sourceId) != 0)
                            {
                                auto& source  = anim.sources_.at(sourceId);
                                auto matrixes = Collada::GetMatrixesFromString(source.dataArray.data);

                                uint16 frameId = 0;
                                for (auto& mat : matrixes)
                                {
                                    if (frames.size() <= frameId)
                                        frames.emplace_back();

                                    vec3 position(mat[3][0], mat[3][1], mat[3][2]);
                                    auto rotation = glm::quat_cast(mat);

                                    frames[frameId].transforms[joint.first].position = position;
                                    frames[frameId].transforms[joint.first].rotation = rotation;

                                    frames[frameId].idTransforms_[joint.second].position = position;
                                    frames[frameId].idTransforms_[joint.second].rotation = rotation;
                                    ++frameId;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }

        for (const auto& frame : frames)
            animationClips[animationClipPair.first].AddFrame(frame);
    }
}
void ColladaDae::ApplyMaterials(Material& material, const std::string& materialId)
{
    if (materialId.empty())
        return;

    const auto& m = data_.libraryMaterials_.materials_[materialId];
    auto& effect  = data_.libraryEffects_.effects_[Collada::GetSource(m.instanceEffect)];

    material.ambient  = Collada::GetVector3(effect.ambient.value);
    material.diffuse  = Collada::GetVector3(effect.diffuse.value);
    material.specular = Collada::GetVector3(effect.specular.value);

    if (!effect.diffuse.texture.empty())
    {
        auto& param = effect.newParams_[effect.diffuse.texture];
        if (param.sampler_)
        {
            const auto& surface = effect.newParams_[param.sampler_.constValue().value_].surface_;

            if (surface)
            {
                const auto& textureFileName = data_.libraryImages_.images_[surface.constValue().initfrom_].initFrom_;

                TextureParameters params;
                params.mimap = GraphicsApi::TextureMipmap::LINEAR;

                material.diffuseTexture = textureLoader_.LoadTexture("Textures/" + textureFileName, params);
            }
        }
        if (param.surface_)
        {
        }
    }

    // material.diffuseTexture
}
Collada::ObjectType ColladaDae::GetType(const std::string& name)
{
    if (data_.libraryGeometries_.geometries_.count(name) != 0)
        return Collada::ObjectType::LIBRARY_GOMETRIES;

    if (data_.libraryControllers_.controllers_.count(name) != 0)
        return Collada::ObjectType::LIBRARY_CONTROLLERS;

    return {};
}
const Collada::Node& ColladaDae::GetNode(const std::string& node_id)
{
    for (const auto& vs : data_.libraryVisualScenes_.visualScenes_)
    {
        for (const auto& node : vs.second.nodes_)
        {
            if (node.second.id_ == node_id)
            {
                return node.second;
            }
            else
            {
                for (const auto& child : node.second.children_)
                {
                    if (child.second->id_ == node_id)
                        return *child.second;

                    return GetNode(*(child.second), node_id);
                }
            }
        }
    }

    return staticNode;
}
const Collada::Node& ColladaDae::GetNode(const Collada::Node& node, const std::string& node_id)
{
    if (node.children_.count(node_id) != 0)
    {
        return *node.children_.at(node_id);
    }

    for (const auto& p : node.children_)
    {
        return GetNode(*p.second, node_id);
    }

    return staticNode;
}
void ColladaDae::Clear()
{
    data_ = Collada::ColladaData();
    idToMeshMap_.clear();
}
}  // namespace WBLoader
}  // namespace GameEngine
