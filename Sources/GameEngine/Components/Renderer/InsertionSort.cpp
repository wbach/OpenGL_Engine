#include "InsertionSort.h"


void GameEngine::SortUpHighToLow(std::vector<Particle>& list, int i, const vec3 & camPosition)
{
	Particle item = list[i];
	auto distance = glm::distance(list[i].position, camPosition);
	int attemptPos = i - 1;
	while (attemptPos != 0 && glm::distance(list[attemptPos - 1].position, camPosition) < distance)
	{
		attemptPos--;
	}
	list.erase(list.begin() + i);
	list[attemptPos] = item;
}

void GameEngine::SortHighToLow(std::vector<Particle>& list, const vec3 & camPosition)
{
	//for (uint32 i = 0; i < list.size(); ++i)
	//{
	//	auto distance = glm::distance(list[i].position, camPosition);
	//	auto distance2 = glm::distance(list[i - 1].position, camPosition);

	//	if (distance > distance2)
	//	{
	//		SortUpHighToLow(list, i, camPosition);
	//	}
	//}

}
