#pragma once
#include "Animation.h"
#include "Joint.h"

#include <unordered_map>

class CMesh;

namespace GameEngine
{
	namespace Animation
	{
		struct Animator
		{
			std::unordered_map<std::string, Animation> animations_;
			std::string current_ = "Animiation_0";
			float time_ = 0.f;
			uint32 currentFrameId_ = 0;

			void Update(CMesh* mesh, float deltaTime);
			void increaseAnimationTime(float deltaTime);
			std::unordered_map<std::string, mat4> calculateCurrentAnimationPose();
			std::unordered_map<std::string, mat4> interpolatePoses(const KeyFrame& previousFrame, const KeyFrame& nextFrame, float progression);

			std::pair<KeyFrame, KeyFrame> getPreviousAndNextFrames();

			void applyPoseToJoints(const std::unordered_map<std::string, mat4>& currentPose, Joint& joint, const mat4& parentTransform);

			float calculateProgression(const KeyFrame& previousFrame, const KeyFrame& nextFrame);
		};
	} // Animation
} // GameEngine
