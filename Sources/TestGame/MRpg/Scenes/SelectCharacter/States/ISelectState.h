#pragma once

namespace MmmoRpg
{
	enum StateStatus
	{
		ON_GOING,
		DONE,
		BACK_TO_PREVIOUS_STATE
	};

	class ISelectState
	{
	public:
		virtual ~ISelectState() {}
		virtual void Update() = 0;
		StateStatus GetStatus() { return status_; }

	protected:
		StateStatus status_ = StateStatus::ON_GOING;
	};
}