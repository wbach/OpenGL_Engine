#include "AnimationUtils.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <Utils/Utils.h>
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

std::optional<AnimationClip> ReadAnimationClip(const File& file, Joint& rootJoint)
{
    if (file.empty())
        return std::nullopt;

    Utils::XmlReader reader;
    reader.Read(file.GetAbsolutePath());
    auto root = reader.Get();

    if (root)
    {
        std::string animationClipName = file.GetBaseName();

        auto animationNameAttribute = root->getAttributeValue("name");
        if (not animationNameAttribute.empty())
        {
            animationClipName = animationNameAttribute;
        }

        AnimationClip animationClip(animationClipName, file.GetInitValue());
        animationClip.SetLength(std::stof(root->attributes_["length"]));

        for (const auto& keyframeNode : reader.Get("KeyFrames")->getChildren())
        {
            KeyFrame keyFrame;
            auto timeStamp     = keyframeNode->getChild("timeStamp");
            keyFrame.timeStamp = std::stof(timeStamp->value_);
            for (const auto& transformNode : keyframeNode->getChild("Transforms")->getChildren())
            {
                JointTransform transform;
                auto jointName = transformNode->getAttributeValue("jointName");
                if (jointName.empty())
                {
                    ERROR_LOG("Joint name empty");
                    continue;
                }

                Read(transformNode->getChild("Position"), transform.position);
                Read(transformNode->getChild("Rotation"), transform.rotation);
                Read(transformNode->getChild("Scale"), transform.scale);

                if (auto joint = rootJoint.getJoint(jointName))
                {
                    keyFrame.transforms.insert({joint->id, transform});
                }
                else
                {
                    ERROR_LOG("Joint \"" + jointName + "\" not found in skeleton. Skeleton root joint name : " + rootJoint.name);
                }
            }
            animationClip.AddFrame(keyFrame);
        }
        return animationClip;
    }

    return std::nullopt;
}

void ExportAnimationClipToFile(const File& file, const AnimationClip& animationClip, const Joint& rootJoint,
                               const std::string& name)
{
    TreeNode rootNode("AnimationClip");
    if (not name.empty())
    {
        rootNode.attributes_.insert({"name", name});
    }
    else if (not animationClip.getName().empty())
    {
        rootNode.attributes_.insert({"name", animationClip.getName()});
    }
    else
    {
        rootNode.attributes_.insert({"name", file.GetBaseName()});
    }

    rootNode.attributes_.insert({"length", std::to_string(animationClip.GetLength())});

    auto& keyFramesNode = rootNode.addChild("KeyFrames");
    keyFramesNode.attributes_.insert({"count", std::to_string(animationClip.GetFrames().size())});

    for (const auto& frame : animationClip.GetFrames())
    {
        auto& keyFrame = keyFramesNode.addChild("KeyFrame");
        keyFrame.addChild("timeStamp", std::to_string(frame.timeStamp.value));
        auto& jontTransformsNode = keyFrame.addChild("Transforms");

        for (const auto& [jointId, transform] : frame.transforms)
        {
            auto& transformNode = jontTransformsNode.addChild("Transform");
            if (jointId == rootJoint.id)
            {
                transformNode.attributes_.insert({"jointName", rootJoint.name});
            }
            else
            {
                auto joint = rootJoint.getJoint(jointId);
                if (joint)
                {
                    transformNode.attributes_.insert({"jointName", joint->name});
                }
            }

            transformNode.addChild(Convert("Position", transform.position));
            transformNode.addChild(Convert("Rotation", transform.rotation));
            transformNode.addChild(Convert("Scale", transform.scale));
        }
    }
    Utils::Xml::Write(file.GetAbsolutePath(), rootNode);
}
std::optional<std::string> IsAnimationClip(const File& file)
{
    Utils::XmlReader reader;
    if (reader.Read(file.GetAbsolutePath()))
    {
        auto root = reader.Get();
        if (root and root->name() == "AnimationClip")
        {
            auto name = root->getAttributeValue("name");
            if (not name.empty())
            {
                return name;
            }
            else
            {
                return file.GetBaseName();
            }
        }
    }
    return std::nullopt;
}
}  // namespace Animation
}  // namespace GameEngine
