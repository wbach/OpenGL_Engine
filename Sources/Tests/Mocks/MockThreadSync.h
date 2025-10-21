#pragma once

#include <gmock/gmock.h>

#include "IThreadSync.h"

namespace Utils
{
namespace Thread
{

class MockThreadSync : public IThreadSync
{
public:
    MOCK_METHOD(uint32, Subscribe, (FrameFunc func, const std::string& label, uint32), (override));

    MOCK_METHOD(ThreadSubscriber*, GetSubscriber, (uint32), (override));

    MOCK_METHOD(void, Unsubscribe, (uint32 id), (override));

    MOCK_METHOD(void, Start, (), (override));

    MOCK_METHOD(void, Stop, (), (override));

    MOCK_METHOD(size_t, SubscribersCount, (), (const, override));

    MOCK_METHOD(void, setTimeMultiplayer, (float), (override));

    MOCK_METHOD(Worker&, AddWorker, (), (override));

    MOCK_METHOD(void, RemoveWorker, (Worker&), (override));
};

}  // namespace Thread
}  // namespace Utils
