#pragma once
#include <Logger/Log.h>
#include <Utils/Container.h>
#include <Utils/IdPool.h>

#include <functional>
#include <mutex>
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
    using Values = std::unordered_map<IdType, Type>;

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

    typename Values::value_type* get(std::function<bool(const typename Values::value_type&)> predicate)
    {
        std::lock_guard<std::mutex> lk(mutex);
        return Utils::get(values, predicate);
    }

    void clear()
    {
        std::lock_guard<std::mutex> lk(mutex);
        values.clear();
    }
    void foreach (std::function<void(IdType, Type&)> f)
    {
        std::lock_guard<std::mutex> lk(mutex);
        for (auto& [id, value] : values)
        {
            f(id, value);
        }
    }

private:
    Values values;
    std::mutex mutex;
    Utils::IdPool& idPool_;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine