#include "BinaryReader.h"
#include <Logger/Log.h>
#include <fstream>
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Animations/KeyFrame.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/Model.h"
#include "Types.h"

namespace GameEngine
{
namespace WBLoader
{
template <class T>
void ReadFile(std::ifstream& file, T& v)
{
    file.read(reinterpret_cast<char*>(&v), sizeof(T));
}

template <class T>
void ReadFile(std::ifstream& file, std::vector<T>& v)
{
    uint32 size = 0;
    file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

    if (size == 0)
        return;
    v.resize(size);
    file.read(reinterpret_cast<char*>(&v[0]), sizeof(T) * size);
}

void ReadFile(std::ifstream& file, std::string& str)
{
    uint32 size = 0;
    file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

    if (size == 0)
        return;

    char buff[512];
    memset(buff, 0, 512);
    file.read(buff, sizeof(char) * size);
    str = std::string(buff);
}

void ReadFile(std::ifstream& file, Animation::KeyFrame& keyFrame)
{
    ReadFile(file, keyFrame.timeStamp);

    uint32 size = 0;
    file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

    if (size > 0)
    {
        for (uint32 i = 0; i < size; ++i)
        {
            std::string name;
            Animation::JointTransform transform;
            ReadFile(file, name);
            ReadFile(file, transform);
            keyFrame.transforms[name] = transform;
        }
    }

    file.read(reinterpret_cast<char*>(&size), sizeof(uint32));
    if (size == 0)
        return;

    for (uint32 i = 0; i < size; ++i)
    {
        uint32 id;
        Animation::JointTransform transform;
        ReadFile(file, id);
        ReadFile(file, transform);
    }
}

void ReadFile(std::ifstream& file, Animation::AnimationClip& clip)
{
    ReadFile(file, clip.name);
    float correctLength = 0;
    ReadFile(file, correctLength);

    uint32 framesSize = 0;
    file.read(reinterpret_cast<char*>(&framesSize), sizeof(uint32));

    if (framesSize == 0)
        return;

    for (uint32 i = 0; i < framesSize; ++i)
    {
        Animation::KeyFrame keyFrame;
        ReadFile(file, keyFrame);
        clip.AddFrame(keyFrame);
    }

    auto diff = fabs(correctLength - clip.GetLength());
    if (diff < std::numeric_limits<float>::epsilon())
    {
        ERROR_LOG("Animation length is diffrent : " + std::to_string(diff));
    }
}

void ReadFile(std::ifstream& file, AnimationClipsMap& animations)
{
    uint32 size = 0;
    file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

    if (size == 0)
        return;

    for (uint32 i = 0; i < size; ++i)
    {
        std::string name;
        ReadFile(file, name);
        ReadFile(file, animations[name]);
    }
}
void ReadFile(std::ifstream& file, Animation::JointTransform& transform)
{
    ReadFile(file, transform.position);
    ReadFile(file, transform.rotation);
}
void ReadFile(std::ifstream& file, Animation::Joint& skeleton)
{
    ReadFile(file, skeleton.id);
    ReadFile(file, skeleton.size);
    ReadFile(file, skeleton.name);
    ReadFile(file, skeleton.transform);
    ReadFile(file, skeleton.invtransform);
//    ReadFile(file, skeleton.animatedTransform);

    uint32 size = 0;
    file.read(reinterpret_cast<char*>(&size), sizeof(uint32));

    if (size == 0)
        return;

    for (uint32 i = 0; i < size; ++i)
    {
        skeleton.children.emplace_back();
        ReadFile(file, skeleton.children.back());
    }
}
std::unique_ptr<Model> ReadBinFile(const std::string& filename, ITextureLoader& textureLoader)
{
    std::ifstream file(filename, std::ios::binary | std::ios::in);

    if (!file.is_open())
        return nullptr;

    std::string file_name;
    ReadFile(file, file_name);
    DEBUG_LOG("file : " + file_name);

    BoundingBox boundingBox;
    ReadFile(file, boundingBox);

    uint32 meshesSize = 0;
    ReadFile(file, meshesSize);

    auto out = std::make_unique<Model>();

    for (uint32 m = 0; m < meshesSize; ++m)
    {
        // std::string name;
        // ReadFile(file, name);

        Material material;
        ReadFile(file, material.name);
        ReadFile(file, material.ambient);
        ReadFile(file, material.diffuse);
        ReadFile(file, material.specular);
        ReadFile(file, material.shineDamper);
        ReadFile(file, material.indexOfRefraction);
        ReadFile(file, material.reflectivity);
        ReadFile(file, material.isTransparency);
        ReadFile(file, material.useFakeLighting);

        std::string textures[4];
        ReadFile(file, textures[0]);
        ReadFile(file, textures[1]);
        ReadFile(file, textures[2]);
        ReadFile(file, textures[3]);

        if (not textures[0].empty())
            material.diffuseTexture = textureLoader.LoadTexture(textures[0], TextureParameters());
        if (not textures[1].empty())
            material.normalTexture = textureLoader.LoadTexture(textures[1], TextureParameters());
        if (not textures[2].empty())
            material.ambientTexture = textureLoader.LoadTexture(textures[2], TextureParameters());
        if (not textures[3].empty())
            material.specularTexture = textureLoader.LoadTexture(textures[3], TextureParameters());

        auto& mesh = out->AddMesh(GraphicsApi::RenderType::TRIANGLES, textureLoader.GetGraphicsApi());
        auto& data = mesh.GetMeshDataRef();
        mesh.SetMaterial(material);

        ReadFile(file, data.bitangents_);
        ReadFile(file, data.bonesWeights_);
        ReadFile(file, data.indices_);
        ReadFile(file, data.instancedMatrixes_);
        ReadFile(file, data.joinIds_);
        ReadFile(file, data.normals_);
        ReadFile(file, data.positions_);
        ReadFile(file, data.tangents_);
        ReadFile(file, data.textCoords_);
        mat4 transform(1.f);
        ReadFile(file, transform);
        mesh.SetTransformMatrix(transform);
    }

    ReadFile(file, out->animationClips_);
   // ReadFile(file, out->skeleton_);

    file.close();

    return out;
}
}  // namespace WBLoader
}  // namespace GameEngine
