#pragma once
#include <Utils/Container.h>
#include <Utils/IdPool.h>

#include <functional>
#include <memory>
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
    Container()
        : idPool_{std::make_shared<Utils::IdPool>()}
    {
    }

    Container(std::shared_ptr<Utils::IdPool> idPool)
        : idPool_{std::move(idPool)}
    {
    }

    Container(const Container&)            = delete;
    Container& operator=(const Container&) = delete;

    using Values = std::unordered_map<IdType, Type>;

    IdType insert(Type type)
    {
        std::lock_guard<std::mutex> lk(mutex);
        auto id = idPool_->getId();
        values.insert({id, std::move(type)});
        return id;
    }
    size_t erase(IdType id)
    {
        std::lock_guard<std::mutex> lk(mutex);
        idPool_->releaseId(id);
        return values.erase(id);
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
    const Type* get(IdType id) const
    {
        std::lock_guard<std::mutex> lk(mutex);
        auto it = values.find(id);
        if (it != values.end())
        {
            return &(it->second);
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
    mutable std::mutex mutex;

    std::shared_ptr<Utils::IdPool> idPool_;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
