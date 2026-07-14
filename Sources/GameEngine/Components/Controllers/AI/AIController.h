#pragma once
#include <deque>
#include <mutex>

#include "AIControllerEvents.h"
#include "GameEngine/Components/Component.h"
#include "GameEngine/Components/ComponentCore.h"

namespace GameEngine
{
namespace Components
{
class HealthComponent;
class CharacterController;
class CharacterStatsComponent;
class RoutineComponent;
class ProfessionComponent;

DECLARE_COMPONENT(AIController)
{
public:
    enum class AITargetingMode
    {
        None,
        AttackEverythingExceptSameRace,
        AttackOnlyHumans,
        HumanStandard,
        GuildStrict,
        CustomWhiteBlackList
    };

    float detectionRadius = 5.0f;
    AITargetingMode targetingMode{AITargetingMode::None};

    // clang-format off
    BEGIN_FIELDS()
        FIELD_ENUM(targetingMode)
        FIELD_FLOAT(detectionRadius)
    END_FIELDS()
    // clang-format on

    using EventQueue = std::deque<AIEvent>;

    AIController(ComponentContext&, GameObject&);
    ~AIController();

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void Init();
    void Update();

    template <typename Event>
    void pushEventToQueue(const Event& event)
    {
        std::lock_guard<std::mutex> lk(eventQueueMutex);
        eventQueue.push_back(event);
    }

    const std::vector<vec3>& getCurrentPath() const;
    void RegisterProfession(ProfessionComponent&);

    float getAttackRadius() const;

private:
    void handleEvent(const AIEvent&);
    void processEvent();
    void runPerceptionCheck();
    HealthComponent* getClosestTarget(float);
    bool isHostile(const CharacterStatsComponent&) const;

private:
    EventQueue eventQueue;
    std::mutex eventQueueMutex;
    bool hasTarget{false};
    HealthComponent* currentTargetHealthComponent{nullptr};
    CharacterStatsComponent* myStats{nullptr};
    RoutineComponent* routineComponent_{nullptr};
    ProfessionComponent* profession_{nullptr};

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
