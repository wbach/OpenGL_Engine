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
