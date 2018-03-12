#pragma once
#include "Joint.h"
#include "Animation.h"
#include <unordered_map>

class CMesh;

namespace GameEngine
{
	namespace Animation
	{
		class Animator
		{
		public:
			Animator();
			void Update(float deltaTime);

		public:
			std::unordered_map<std::string, Animation> animations_;
			std::string current_ = "Animiation_0";
			Joint* rootJoint_;
			float time_ = 0.f;

		private:
			void increaseAnimationTime(float deltaTime);
			std::pair<KeyFrame, KeyFrame> getPreviousAndNextFrames();
			float calculateProgression(const KeyFrame& previousFrame, const KeyFrame& nextFrame);
			std::unordered_map<std::string, mat4> calculateCurrentAnimationPose();
			std::unordered_map<std::string, mat4> interpolatePoses(const KeyFrame& previousFrame, const KeyFrame& nextFrame, float progression);
			void applyPoseToJoints(const std::unordered_map<std::string, mat4>& currentPose, Joint& joint, const mat4& parentTransform);

		private:
			uint32 currentFrameId_ = 0;
		};
	} // Animation
} // GameEngine
