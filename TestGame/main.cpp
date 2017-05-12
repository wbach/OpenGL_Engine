#include "MainScene.h"
#include "../GameEngine/Engine/Engine.h"

//#include "../GameEngine/Objects/RenderAble/RenderObject.h"
//
//void MemoryTest()
//{
//    static CEngine engine;
//
//	class TestScene : public CScene
//	{
//	public:
//		TestScene()
//		{
//		}
//		virtual int		Initialize()
//		{
//			m_Camera = std::make_unique<CFirstPersonCamera>(&engine.m_InputManager, &engine.m_DisplayManager);
//			return 0;
//		}
//		virtual void	PostInitialize() {};
//		virtual int		Update()
//		{
//			if (m_Camera != nullptr)
//			{
//				m_Camera->CalculateInput();
//				m_Camera->Move();
//			}
//			return 0;
//		}
//
//	};
//	engine.Init();
//
//	long long i = 0;
//	while(1)
//	{
//		std::cout << "Reply: "<< i++ << std::endl;
//
//		if (engine.m_Scene != nullptr)
//			engine.m_Scene.reset();
//		engine.m_Scene = std::make_unique<TestScene>();
//		engine.PreperaScene();
//	}
//}

void TestGameStart()
{
    CEngine engine;
	engine.Init();
    engine.m_Scene = std::make_unique<MainScene>(engine);
	engine.PreperaScene();
	engine.GameLoop();
}

int main(int argc, char* argv[])
{	
	//MemoryTest();
	CLogger::Instance().EnableLogs();
	TestGameStart();
	return 0;
}
