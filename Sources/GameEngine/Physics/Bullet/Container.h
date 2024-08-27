#pragma once
#include <Utils/IdPool.h>
#include <mutex>
#include <functional>
#include <Logger/Log.h>
#include <unordered_map>

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
template <class Type>
class Container
{
public:
    Container(Utils::IdPool& idPool)
        : idPool_{idPool}
    {
    }
    IdType insert(Type type)
    {
        std::lock_guard<std::mutex> lk(mutex);
        auto id = idPool_.getId();
        values.insert({id, std::move(type)});
        return id;
    }
    void erase(IdType id)
    {
        std::lock_guard<std::mutex> lk(mutex);
        idPool_.releaseId(id);
        values.erase(id);
    }
    Type* get(IdType id)
    {
        std::lock_guard<std::mutex> lk(mutex);
        auto it = values.find(id);
        if (it != values.end())
        {
            auto& [_, v] = *it;
            return &v;
        }
        return nullptr;
    }
    void clear()
    {
        std::lock_guard<std::mutex> lk(mutex);
        values.clear();
    }
    void foreach (std::function<void(Type&)> f)
    {
        std::lock_guard<std::mutex> lk(mutex);
        for (auto& [_, value] : values)
        {
            f(value);
        }
    }

private:
    std::unordered_map<IdType, Type> values;
    std::mutex mutex;
    Utils::IdPool& idPool_;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine