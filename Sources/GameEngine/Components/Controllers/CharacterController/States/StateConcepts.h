#pragma once
#include "Armed.h"
#include "ArmedChangeStateBase.h"
#include "Crouch.h"
#include "Disarmed.h"

namespace GameEngine
{
namespace Components
{
template <typename T>
concept IsCrouchArmed = IsArmed<T> and IsCrouch<T>;

template <typename T>
concept IsCrouchDisarmed = IsDisarmed<T> and IsCrouch<T>;

template <typename T>
concept IsCrouchArmedChange = IsArmedChange<T> and IsCrouch<T>;
}  // namespace Components
}  // namespace GameEngine
