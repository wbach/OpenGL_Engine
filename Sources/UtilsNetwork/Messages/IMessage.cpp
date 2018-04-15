#include "IMessage.h"

void Network::CopyToArray(char * dest, const std::string & str)
{
	int i = 0;
	for (auto c : str)
	{
		if (i >= arraySize - 1)
			break;

		dest[i] = c;
		++i;
	}
}
