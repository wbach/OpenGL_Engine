#pragma once
#include "KeyFrame.h"

namespace GameEngine
{
	namespace Animation
	{
		class AnimationClip
		{
		public:
			AnimationClip();
			AnimationClip(const std::string& name);
			inline float GetLength() const;
			inline void AddFrame(const KeyFrame& frame);
			inline const std::vector<KeyFrame>& GetFrames() const;
			
		public:
			std::string name;

		private:
			float length;
			std::vector<KeyFrame> frames;
		};
		
		inline AnimationClip::AnimationClip()
			: AnimationClip("No name")
		{
		}

		inline AnimationClip::AnimationClip(const std::string& name)
			: name(name)
		{
		}

		float AnimationClip::GetLength() const
		{
			return length;
		}

		void AnimationClip::AddFrame(const KeyFrame& frame)
		{
			frames.push_back(frame);
			length = frame.timeStamp;
		}

		inline const std::vector<KeyFrame>& AnimationClip::GetFrames() const
		{
			return frames;
		}

	} // Animation
} // GameEngine
