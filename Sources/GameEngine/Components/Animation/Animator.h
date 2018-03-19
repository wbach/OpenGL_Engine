#pragma once
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/AbstractComponent.h"
#include <unordered_map>

class CMesh;

namespace GameEngine
{
	namespace Components
	{
		class Animator : public AbstractComponent
		{
		public:
			Animator();
			void Update();
			void SetSkeleton(Animation::Joint* skeleton);
			virtual void ReqisterFunctions() override;

		public:
			std::unordered_map<std::string, Animation::AnimationClip> animationClips_;
			std::string current_ = "DefaultAnimationClip";
			Animation::Joint* rootJoint_;
			float currentTime_ = 0.f;
			float animationSpeed_ = 1.f;

		private:
			void GetSkeletonAndAniations();
			bool IsReady();
			void increaseAnimationTime();
			std::pair<Animation::KeyFrame, Animation::KeyFrame> getPreviousAndNextFrames();
			float calculateProgression(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame);
			std::unordered_map<std::string, mat4> calculateCurrentAnimationPose();
			std::unordered_map<std::string, mat4> interpolatePoses(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame, float progression);
			void applyPoseToJoints(const std::unordered_map<std::string, mat4>& currentPose, Animation::Joint& joint, const mat4& parentTransform);

		private:
			uint32 currentFrameId_ = 0;
		
		public:
			static ComponentsType type;
		};
	} // Animation
} // GameEngine
