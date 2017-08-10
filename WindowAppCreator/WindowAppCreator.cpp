#include "WindowAppCreator.h"

#ifdef _WIN32
#include "Items/windows/WindowsItemsBuilder.h"
#else

#endif

WindowAppCreator::WindowAppCreator()
#ifdef _WIN32
	: itemBuilder(CreateItemBuilderPtr<WidowsItemBuidler>())
#else

#endif
{

}

void WindowAppCreator::Init()
{
	//for (auto& item : items)
	//	item.second->Init();
}


void WindowAppCreator::Run()
{
	bool running = true;
	while (true)
	{
		if (items.empty())
			return;

		for (auto& item : items)
		{
			auto message = item.second->Update();

			switch (message)
			{
			case ItemMessage::QUIT:
				running = false;
				break;
			case ItemMessage::FORCE_QUIT:
				return;
			}
		}
	}
		
}
