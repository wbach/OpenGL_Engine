#include "Gwint.h"

int main(int argc, char* argv[])
{
	//while (1)
	//{
	//	std::string msg;
	//	auto response = sdl.GetMessage(msg);
	//	if (response)
	//	{
	//		std::cout << msg << std::endl;
	//		break;
	//	}
	//}
	CLogger::Instance().EnableLogs();
	srand((uint)time(NULL));
	CGwintGame game;
	game.Start();
	return 0;
}

