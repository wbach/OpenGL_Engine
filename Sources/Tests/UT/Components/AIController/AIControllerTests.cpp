#include <GameEngine/Components/Controllers/AI/AIController.h>
#include <GameEngine/Components/Controllers/CharacterController/CharacterController.h>
#include <GameEngine/Components/Gameplay/HealthComponent.h>

//#include "Tests/UT/EngineBasedTest.h"
#include "Tests/UT/Components/BaseComponent.h"
using namespace GameEngine;
using namespace GameEngine::Components;
using namespace ::testing;

// MATCHER_P(CollisionDetectionActionMatcher, action, "Action matcher for CollisionDetection")
// {
//     return (arg.action == action);
// }

struct AIControllerTests : public BaseComponentTestSchould
{
    AIControllerTests()
        : BaseComponentTestSchould()
        , sut_(context_, *obj_)
    {
        // auto enemyObj = scene.CreateGameObject("Enemy");
        // enemyObj->AddComponent<HealthComponent>();
        // scene.AddGameObject(std::move(enemyObj));

        scene.ProcessEvents();

        obj_->AddComponent<CharacterController>();

        sut_.ReqisterFunctions();

        componentController_.CallFunctions(FunctionType::Awake);
        componentController_.CallFunctions(FunctionType::LateAwake);
        componentController_.CallFunctions(FunctionType::OnStart);
    }

    void SetUp() override
    {

    }

    AIController sut_;
};

TEST_F(AIControllerTests, Dummy)
{
    componentController_.CallFunctions(FunctionType::Update);
    componentController_.CallFunctions(FunctionType::Update);
}