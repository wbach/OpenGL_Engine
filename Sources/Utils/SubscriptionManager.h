#pragma once
#include "IdPool.h"
#include "Types.h"

namespace Utils
{
using ObjectId = IdType;
using SubId    = IdType;

template <typename Callback>
class SubscriptionManager
{
public:
    IdType subscribe(ObjectId objId, Callback cb)
    {
        auto sId = idPool_.getId();

        subscriptions[objId][sId] = cb;
        subToOwnerLookup[sId]     = objId;

        return sId;
    }

    void notifyAndRemoveObject(ObjectId objId)
    {
        auto it = subscriptions.find(objId);
        if (it != subscriptions.end())
        {
            for (auto const& [sId, callback] : it->second)
            {
                callback();
                subToOwnerLookup.erase(sId);
            }
            subscriptions.erase(it);
        }
    }

    void unsubscribe(SubId sId)
    {
        auto itLookup = subToOwnerLookup.find(sId);
        if (itLookup != subToOwnerLookup.end())
        {
            ObjectId objId = itLookup->second;
            subscriptions[objId].erase(sId);
            if (subscriptions[objId].empty())
            {
                subscriptions.erase(objId);
            }
            subToOwnerLookup.erase(itLookup);
        }
    }

private:
    Utils::IdPool idPool_;

    std::unordered_map<ObjectId, std::unordered_map<SubId, Callback>> subscriptions;
    std::unordered_map<SubId, ObjectId> subToOwnerLookup;
};
}  // namespace Utils
