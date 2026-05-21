#pragma once
#include <deque>

#include "AIControllerEvents.h"
#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
class CharacterController;

DECLARE_COMPONENT(AIController)
{
public:
    using EventQueue = std::deque<AIEvent>;

    AIController(ComponentContext&, GameObject&);

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
    void handleEvent(const AIEvent&);

    const std::vector<vec3>& getCurrentPath() const;

private:
    void processEvent();

private:
    EventQueue eventQueue;
    std::mutex eventQueueMutex;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
