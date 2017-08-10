#pragma once
#ifdef _WIN32
#include "../WindowsItem.h"

namespace Win32
{
	class Win32_Button : public virtual Win32_Item
	{
	public:
		Win32_Button();
		Win32_Button(int, int y, int width, int height, const std::string& text, std::function<void()>);
		virtual ItemMessage Init() override;
		virtual ItemMessage Update() override;

	public:

	private:
		void SetLocalVaribles();

	};
}//Win32
#endif