#include "AnimationUtils.h"

#include <Utils/GLM/GLMUtils.h>
#include <Utils/XML/XMLUtils.h>
#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>

namespace GameEngine
{
namespace Animation
{
mat4 GetLocalTransform(const JointTransform& jt)
{
    return glm::translate(jt.position) * glm::mat4_cast(jt.rotation);
}
JointTransform GetJointTransform(const glm::mat4& mat)
{
    return {vec3(mat[3][0], mat[3][1], mat[3][2]), glm::quat_cast(mat)};
}
const Joint* GetJoint(const Joint& from, uint32 fid)
{
    if (from.id == fid)
        return &from;

    for (auto& j : from.children)
    {
        auto f = GetJoint(j, fid);
        if (f != nullptr)
            return f;
    }
    return nullptr;
}
void CalcInverseBindTransform(Joint& joint, const mat4& parentBindTransform)
{
    mat4 bindTransform = parentBindTransform * joint.transform;
    joint.invtransform = glm::inverse(bindTransform);

    for (Joint& child : joint.children)
    {
        CalcInverseBindTransform(child, bindTransform);
    }
}
JointTransform Interpolate(const JointTransform& frameA, const JointTransform& frameB, float progress)
{
    JointTransform out;
    out.position = glm::mix(frameA.position, frameB.position, progress);
    out.rotation = Utils::Interpolate(frameA.rotation, frameB.rotation, progress);
    out.scale    = glm::mix(frameA.scale, frameB.scale, progress);
    return out;
}

AnimationClip ReadAnimationClip(const File& file, Joint& rootJoint)
{
    Utils::XmlReader reader;
    reader.Read(file.GetAbsoultePath());
    auto root = reader.Get();

    if (root)
    {
        AnimationClip animationClip(root->attributes_["name"]);
        animationClip.filePath = file.GetInitValue();
        animationClip.SetLength(std::stof(root->attributes_["length"]));

        for (const auto& keyframeNode : reader.Get("KeyFrames")->GetChildren())
        {
            KeyFrame keyFrame;
            auto timeStamp     = keyframeNode->GetChild("timeStamp");
            keyFrame.timeStamp = std::stof(timeStamp->value_);
            for (const auto& transformNode : keyframeNode->GetChild("Transforms")->GetChildren())
            {
                JointTransform transform;
                auto jointName    = transformNode->attributes_.at("jointName");
                auto positionNode = transformNode->GetChild("Position");
                if (positionNode)
                {
                    transform.position = Utils::ReadVec3(*positionNode);
                }

                auto rotationNode = transformNode->GetChild("Rotation");
                if (rotationNode)
                {
                    transform.rotation = Utils::ReadQuat(*rotationNode);
                }

                auto scaleNode = transformNode->GetChild("Scale");
                if (scaleNode)
                {
                    transform.scale = Utils::ReadVec3(*scaleNode);
                }
                uint32 jointId(rootJoint.id);
                if (rootJoint.name != jointName)
                {
                    auto jointPtr = rootJoint.getChild(jointName);
                    if (jointPtr)
                    {
                        jointId = jointPtr->id;
                    }
                }
                keyFrame.transforms.insert({jointId, transform});
            }
            animationClip.AddFrame(keyFrame);
        }
        return animationClip;
    }

    return AnimationClip();
}

void ExportAnimationClipToFile(const File& file, const AnimationClip& animationClip, Joint& rootJoint)
{
    Utils::XmlNode rootNode("AnimationClip");

    rootNode.attributes_.insert({"name", animationClip.name.empty() ? "NoName" : animationClip.name});
    rootNode.attributes_.insert({"length", std::to_string(animationClip.GetLength())});

    auto& keyFramesNode = rootNode.AddChild("KeyFrames");
    keyFramesNode.attributes_.insert({"count", std::to_string(animationClip.GetFrames().size())});

    for (const auto& frame : animationClip.GetFrames())
    {
        auto& keyFrame = keyFramesNode.AddChild("KeyFrame");
        keyFrame.AddChild("timeStamp", std::to_string(frame.timeStamp));
        auto& jontTransformsNode = keyFrame.AddChild("Transforms");

        for (const auto& transformPair : frame.transforms)
        {
            auto& transformNode = jontTransformsNode.AddChild("Transform");
            if (transformPair.first == rootJoint.id)
            {
                transformNode.attributes_.insert({"jointName", rootJoint.name});
            }
            else
            {
                auto joint = rootJoint.getChild(transformPair.first);
                if (joint)
                {
                    transformNode.attributes_.insert({ "jointName", joint->name });
                }
            }

            const auto& transform = transformPair.second;
            transformNode.AddChild(Utils::Convert("Position", transform.position));
            transformNode.AddChild(Utils::Convert("Rotation", transform.rotation));
            transformNode.AddChild(Utils::Convert("Scale", transform.scale));
        }
    }
    Utils::Xml::Write(file.GetAbsoultePath(), rootNode);
}
}  // namespace Animation
}  // namespace GameEngine
