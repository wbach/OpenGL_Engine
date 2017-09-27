#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Engine/Configuration.h"

#include "WindowAppCreator.h"
#include "Logger/Log.h"
#include <fstream>
#include "FreeImage.h"
#include <thread>
#include "Scene/Scene.h"


WindowAppCreator app;

void ExitFunction()
{
	exit(0);
}

void TestFunction()
{

}

void Start()
{
	if (app.itemBuilder == nullptr)
		return;

	app.items["MainWindow"] = app.itemBuilder->CreateWindowItem(640, 480, "Moj tytu³",
	{
		{ActionsType::ON_EXIT, std::bind(ExitFunction)}
	});

	app.items["MainWindow"]->Init();

	app.items["testButton"] = app.itemBuilder->CreateButton(10, 10, 100, 30, "Przycisk haxd", TestFunction);
	app.items["MainWindow"]->AddChild(app.items["testButton"]);
	app.items["testButton"]->Init();

	app.Init();
	app.Run();
}


void SceneViewer()
{
    CEngine engine;
	engine.Init();
    engine.scene = std::make_unique<MainScene>(engine);
	engine.PreperaScene();
	engine.GameLoop();
}

int main(int argc, char* argv[])
{	
	std::thread t(Start);
	t.detach();
	CLogger::Instance().EnableLogs();
	SceneViewer();
	return 0;
}
