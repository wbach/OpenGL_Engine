#include "Animator.h"
#include "GameEngine/Resources/Models/Mesh.h"

namespace GameEngine
{
	namespace Animation
	{
		void Animator::Update(CMesh * mesh, float deltaTime)
		{
			if (mesh == nullptr || current_.empty() || animations_.count(current_) == 0)
				return;

			increaseAnimationTime(deltaTime);
			auto currentPose = calculateCurrentAnimationPose();
			applyPoseToJoints(currentPose, mesh->rootJoint_, mat4());
		}

		void Animator::increaseAnimationTime(float deltaTime)
		{
			auto l = animations_[current_].length;

			time_ += deltaTime;
			if (time_ > l)
				time_ = fmod(time_, l);
		}

		std::unordered_map<std::string, mat4> Animator::calculateCurrentAnimationPose() {
			auto frames = getPreviousAndNextFrames();
			float progression = calculateProgression(frames.first, frames.second);
			return interpolatePoses(frames.first, frames.second, progression);
		}

		std::unordered_map<std::string, mat4> Animator::interpolatePoses(const KeyFrame & previousFrame, const KeyFrame & nextFrame, float progression)
		{
			std::unordered_map<std::string, mat4> currentPose;
			for (const auto& pair : previousFrame.transforms)
			{
				auto& jointName = pair.first;
				if (previousFrame.transforms.count(jointName) == 0 || nextFrame.transforms.count(jointName) == 0)
					continue;

				JointTransform previousTransform = previousFrame.transforms.at(jointName);
				JointTransform nextTransform = nextFrame.transforms.at(jointName);
				JointTransform currentTransform = JointTransform::Interpolate(previousTransform, nextTransform, progression);
				currentPose[jointName] = currentTransform.GetLocalTransform();
			}
			return currentPose;
		}

		std::pair<KeyFrame, KeyFrame> Animator::getPreviousAndNextFrames()
		{
			const auto& allFrames = animations_[current_].frames;

			KeyFrame previousFrame = allFrames[0];
			KeyFrame nextFrame = allFrames[0];
			for (int i = 1; i < allFrames.size(); i++)
			{
				nextFrame = allFrames[i];
				if (nextFrame.timeStamp > time_)
					break;

				previousFrame = allFrames[i];
			}
			return { previousFrame, nextFrame };
		}

		void Animator::applyPoseToJoints(const std::unordered_map<std::string, mat4>& currentPose, Joint& joint, const mat4& parentTransform)
		{
			if (currentPose.count(joint.name) == 0)
				return;

			const auto& currentLocalTransform = currentPose.at(joint.name);
			auto currentTransform = parentTransform * currentLocalTransform;
			joint.animatedTransform = currentTransform * joint.inverseBindTransform;

			for (Joint& childJoint : joint.children)
			{
				applyPoseToJoints(currentPose, childJoint, currentTransform);
			}
		}

		float Animator::calculateProgression(const KeyFrame& previousFrame, const KeyFrame& nextFrame)
		{
			float totalTime = nextFrame.timeStamp - previousFrame.timeStamp;
			float currentTime = time_ - previousFrame.timeStamp;
			return currentTime / totalTime;
		}
	} // Animation
} // GameEngine
