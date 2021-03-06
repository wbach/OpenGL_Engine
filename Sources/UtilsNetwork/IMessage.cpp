#include "IMessage.h"

void Network::CopyToArray(char* dest, const std::string& str, int size)
{
    memset(dest, 0, size);

    int i = 0;
    for (auto c : str)
    {
        if (i >= size)
            break;

        dest[i] = c;
        ++i;
    }
}
