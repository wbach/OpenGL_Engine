#pragma once
#include <memory>

#include "ControllersTypes.h"
#include "Logger/Log.h"
#include <magic_enum/magic_enum.hpp>

namespace common
{
namespace Controllers
{
class IController
{
public:
    IController(Controllers::Types type)
        : type_(type)
    {
    }
    virtual ~IController()
    {
    }
    virtual void Update(float dt) = 0;
    Controllers::Types GetType()
    {
        return type_;
    }

private:
    Controllers::Types type_;
};

typedef std::shared_ptr<IController> IControllerPtr;
template <class T>
std::shared_ptr<T> castControllerAs(IControllerPtr ptr)
{
    auto msg = std::static_pointer_cast<T>(ptr);
    if (msg == nullptr)
    {
        LOG_ERROR << "Cant cast type : " << magic_enum::enum_name(ptr->GetType());
        return nullptr;
    }
    return msg;
}
template <class T>
T* castControllerAs(IController* ptr)
{
    auto msg = static_cast<T*>(ptr);
    if (msg == nullptr)
    {
        LOG_ERROR << "Cant cast type : " << magic_enum::enum_name(ptr->GetType());
        return nullptr;
    }
    return msg;
}
}  // namespace Controllers
}  // namespace common