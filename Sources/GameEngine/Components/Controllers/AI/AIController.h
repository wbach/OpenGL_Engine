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
    float getMeleAttackRadius() const;
    std::optional<float> getRangeAttackRadius() const;

private:
    void handleEvent(const AIEvent&);
    void processEvent();

private:
    EventQueue eventQueue;
    std::mutex eventQueueMutex;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    const Impl& getImpl() const;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
