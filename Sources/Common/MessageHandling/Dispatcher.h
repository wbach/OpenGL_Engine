#pragma once
#include <unordered_map>
#include "AbstractHandler.h"

namespace common
{
typedef std::unordered_map<std::string, AbstractHandlerPtr> Handlers;

class Dispacher
{
public:
    void SetHandlers(Handlers handlers);
    void AddHandlers(Handlers handlers);
    void AddHandler(const std::string& label, AbstractHandler* handlerPtr);
    void RemoveHandler(const std::string& label);
    void Dispatch(const Network::BoxMessage& message);
    void Clear();

private:
    Handlers handlers_;
};
}  // namespace common
