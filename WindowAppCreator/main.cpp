#include "WindowAppCreator.h"

WindowAppCreator app;

void Start()
{
	if (app.itemBuilder == nullptr)
		return;

	app.items["MainWindow"] = app.itemBuilder->CreateSingleWindow();
}

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Start();
	system("pause");
	return 0;
}

#else
int main(int argc, char** argv)
{
	return 0;
}
#endif

