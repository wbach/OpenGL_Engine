#include "DialogContext.h"

#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
void setAnimation(GameObject& gameObject, StartDialogEvent::Role role)
{
    if (auto characterController = gameObject.GetComponent<Components::CharacterController>())
    {
        characterController->pushEventToQueue(StartDialogEvent{.role = role});
    }
};
}  // namespace GameEngine
