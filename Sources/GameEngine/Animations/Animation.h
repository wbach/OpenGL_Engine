#pragma once
#include "KeyFrame.h"

namespace GameEngine
{
	namespace Animation
	{
		class Animation
		{
		public:
			inline float GetLength() const;
			inline void AddFrame(const KeyFrame& frame);
			inline const std::vector<KeyFrame>& GetFrames() const;

		private:
			float length;
			std::vector<KeyFrame> frames;
		};
		
		float Animation::GetLength() const
		{
			return length;
		}

		void Animation::AddFrame(const KeyFrame& frame)
		{
			frames.push_back(frame);
			length = frame.timeStamp;
		}

		inline const std::vector<KeyFrame>& Animation::GetFrames() const
		{
			return frames;
		}

	} // Animation
} // GameEngine
