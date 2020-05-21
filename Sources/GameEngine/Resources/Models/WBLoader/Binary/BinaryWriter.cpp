#include "BinaryWriter.h"
#include <Logger/Log.h>
#include <fstream>
#include <vector>
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Animations/KeyFrame.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/Models/Model.h"
#include "Types.h"

namespace GameEngine
{
namespace WBLoader
{
std::string CreateBinPath(const std::string& filename)
{
    return EngineConf.files.data + "/Binary/" + filename + ".bin";
}
template <class T>
void WriteToFile(std::fstream& file, const std::vector<T>& v)
{
    uint32 size = v.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(uint32));

    if (!v.empty())
        file.write(reinterpret_cast<const char*>(&v[0]), sizeof(T) * v.size());
}
template <class T>
void WriteToFile(std::fstream& file, T value)
{
    file.write(reinterpret_cast<const char*>(&value), sizeof(T));
}
void WriteToFile(std::fstream& file, const std::string& str)
{
    uint32 size = str.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(uint32));
    if (!str.empty())
        file.write(str.c_str(), sizeof(char) * str.size());
}
void WriteToFile(std::fstream& file, Texture* texture)
{
    if (texture == nullptr)
    {
        WriteToFile(file, std::string());
    }
    else
    {
        WriteToFile(file, texture->GetFile()->GetDataRelativeDir());
    }
}
void WriteToFile(std::fstream& file, const Animation::JointTransform& jointTransform)
{
    WriteToFile(file, jointTransform.position);
    WriteToFile(file, jointTransform.rotation);
}
void WriteToFile(std::fstream& file, const Animation::KeyFrame& keyFrame)
{
    WriteToFile(file, keyFrame.timeStamp);
    WriteToFile(file, keyFrame.transforms.size());
    for (const auto& jointTransform : keyFrame.transforms)
    {
        WriteToFile(file, jointTransform.first);
        WriteToFile(file, jointTransform.second);
    }
    WriteToFile(file, keyFrame.idTransforms_.size());
    for (const auto& jointTransform : keyFrame.idTransforms_)
    {
        WriteToFile(file, jointTransform.first);
        WriteToFile(file, jointTransform.second);
    }
}
void WriteToFile(std::fstream& file, const Animation::AnimationClip& clip)
{
    WriteToFile(file, clip.name);
    WriteToFile(file, clip.GetLength());
    const auto& frames = clip.GetFrames();
    WriteToFile(file, frames.size());
    for (const auto& keyFrame : frames)
    {
        WriteToFile(file, keyFrame);
    }
}
void WriteToFile(std::fstream& file, const Animation::Joint& joint)
{
    WriteToFile(file, joint.id);
    WriteToFile(file, joint.size);
    WriteToFile(file, joint.name);
    WriteToFile(file, joint.transform);
    WriteToFile(file, joint.invtransform);
    WriteToFile(file, joint.animatedTransform);
    WriteToFile(file, joint.children.size());
    for (const auto& child : joint.children)
    {
        WriteToFile(file, child);
    }
}
void WriteToFile(std::fstream& file, const AnimationClipsMap& animations)
{
    WriteToFile(file, animations.size());
    for (const auto& clip : animations)
    {
        WriteToFile(file, clip.first);
        WriteToFile(file, clip.second);
    }
}
void WriteToFile(std::fstream& file, const GraphicsApi::MeshRawData& data, const Material& material,
                 const mat4& objTransform)
{
    WriteToFile(file, material.name);
    WriteToFile(file, material.ambient);
    WriteToFile(file, material.diffuse);
    WriteToFile(file, material.specular);
    WriteToFile(file, material.shineDamper);
    WriteToFile(file, material.indexOfRefraction);
    WriteToFile(file, material.reflectivity);
    WriteToFile(file, material.isTransparency);
    WriteToFile(file, material.useFakeLighting);
    WriteToFile(file, material.diffuseTexture);
    WriteToFile(file, material.normalTexture);
    WriteToFile(file, material.ambientTexture);
    WriteToFile(file, material.specularTexture);
    WriteToFile(file, data.bitangents_);
    WriteToFile(file, data.bonesWeights_);
    WriteToFile(file, data.indices_);
    WriteToFile(file, data.instancedMatrixes_);
    WriteToFile(file, data.joinIds_);
    WriteToFile(file, data.normals_);
    WriteToFile(file, data.positions_);
    WriteToFile(file, data.tangents_);
    WriteToFile(file, data.textCoords_);
    WriteToFile(file, objTransform);
}
void CreateBinFile(const std::unique_ptr<Model>& model, const std::string& filename)
{
    if (!EngineConf.useBinaryLoading)
        return;

    std::fstream binaryFile(CreateBinPath(filename), std::ios::binary | std::ios::out);

    WriteToFile(binaryFile, model->GetFile().GetDataRelativeDir());
    WriteToFile(binaryFile, model->GetScaleFactor());
    WriteToFile(binaryFile, model->GetMeshes().size());
    for (auto& mesh : model->GetMeshes())
    {
        WriteToFile(binaryFile, mesh.GetCMeshDataRef(), mesh.GetMaterial(), mesh.GetMeshTransform());
    }
    WriteToFile(binaryFile, model->animationClips_);
    WriteToFile(binaryFile, model->skeleton_);

    binaryFile.close();
}
}  // WBLoader
}  // namespace GameEngine
