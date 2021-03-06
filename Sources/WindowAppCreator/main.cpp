#include "WindowAppCreator.h"

#ifdef _WIN32
#include <windows.h>
#endif
WindowAppCreator app;

void TestFunction()
{
#ifdef _WIN32
	MessageBox(NULL, "Nacisnales prycisk! xd", "Super!", MB_ICONEXCLAMATION);
#endif
}

void TestFunction2()
{
#ifdef _WIN32
	MessageBox(NULL, "Nacisnales prycisk   2    \n\n! xd", "Super!", MB_ICONEXCLAMATION);
#endif
}

void TestFunction3()
{
#ifdef _WIN32
	MessageBox(NULL, "Nacisnales prycisk   3    \n\n! xd", "Super!", MB_ICONEXCLAMATION);
#endif
}
#include <functional>

void ExitFunction()
{
	exit(0);
}

void Start()
{
	if (app.itemBuilder == nullptr)
		return;

	app.items["MainWindow"] = app.itemBuilder->CreateWindowItem(640, 480, "Moj tytu�", 
	{
		{ActionsType::ON_EXIT, std::bind(ExitFunction)}
	});

	app.items["MainWindow"]->Init();

	app.items["testButton"] = app.itemBuilder->CreateButton(10, 10, 100, 30, "Przycisk haxd", TestFunction);
	app.items["MainWindow"]->AddChild(app.items["testButton"]);
	app.items["testButton"]->Init();

	app.items["testButton2"] = app.itemBuilder->CreateButton(10, 100, 100, 30, "Przycisk 2 haxd", TestFunction2);
	app.items["MainWindow"]->AddChild(app.items["testButton2"]);
	app.items["testButton2"]->Init();

	app.Init();
	app.Run();
}

#ifdef _WIN32


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

