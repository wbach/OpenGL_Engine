#include <Utils/Time/TimerService.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>

using namespace std::chrono_literals;

class TimerServiceTest : public ::testing::Test
{
protected:
    Utils::Time::TimerService service;
};

TEST_F(TimerServiceTest, SingleTimerExecutes)
{
    std::promise<void> promise;
    auto future = promise.get_future();

    service.timer(50ms, [&]() { promise.set_value(); });

    auto status = future.wait_for(200ms);
    EXPECT_EQ(status, std::future_status::ready);
}

TEST_F(TimerServiceTest, MultipleTimersInOrder)
{
    std::atomic<int> counter{0};

    service.timer(100ms, [&]() { counter++; });
    service.timer(50ms, [&]() { counter++; });

    std::this_thread::sleep_for(75ms);
    EXPECT_EQ(counter.load(), 1);

    std::this_thread::sleep_for(50ms);
    EXPECT_EQ(counter.load(), 2);
}

TEST_F(TimerServiceTest, PeriodicTimerExecutesMultipleTimes)
{
    std::atomic<int> executions{0};
    service.periodicTimer(20ms, [&]() { executions++; });
    std::this_thread::sleep_for(80ms);

    EXPECT_GE(executions.load(), 3);
}

TEST_F(TimerServiceTest, CancelTimerPreventsExecution)
{
    bool executed = false;
    auto id       = service.timer(50ms, [&]() { executed = true; });

    service.cancel(id);
    std::this_thread::sleep_for(100ms);

    EXPECT_FALSE(executed);
}

TEST_F(TimerServiceTest, StressTestRapidAddition)
{
    std::atomic<int> count{0};
    const int numTimers = 100;

    for (int i = 0; i < numTimers; ++i)
    {
        service.timer(10ms, [&]() { count++; });
    }

    std::this_thread::sleep_for(200ms);
    EXPECT_EQ(count.load(), numTimers);
}

TEST_F(TimerServiceTest, AddingEarlierTimerWhileSleeping)
{
    std::atomic<bool> longTimerCalled{false};
    std::atomic<bool> shortTimerCalled{false};

    service.timer(1000ms, [&]() { longTimerCalled.store(true); });

    std::this_thread::sleep_for(20ms);

    service.timer(50ms, [&]() { shortTimerCalled.store(true); });

    std::this_thread::sleep_for(100ms);

    EXPECT_TRUE(shortTimerCalled.load());
    EXPECT_FALSE(longTimerCalled.load());
}

TEST_F(TimerServiceTest, MultithreadedAdditionAndExecution)
{
    const int threadsCount    = 10;
    const int timersPerThread = 50;
    std::atomic<int> executionCount{0};
    std::vector<std::thread> producers;

    for (int i = 0; i < threadsCount; ++i)
    {
        producers.emplace_back(
            [&]()
            {
                for (int j = 0; j < timersPerThread; ++j)
                {
                    service.timer(10ms + (j * 1ms), [&executionCount]() { executionCount++; });
                }
            });
    }

    for (auto& t : producers)
    {
        t.join();
    }

    std::this_thread::sleep_for(500ms);

    EXPECT_EQ(executionCount.load(), threadsCount * timersPerThread);
}

TEST_F(TimerServiceTest, RapidAddAndCancelCycle)
{
    const int iterations = 10000;
    std::atomic<int> callCount{0};

    for (int i = 0; i < iterations; ++i)
    {
        auto id = service.timer(1ms, [&]() { callCount++; });

        if (i % 2 == 0)
        {
            service.cancel(id);
        }

        if (i % 100 == 0)
        {
            std::this_thread::sleep_for(1ms);
        }
    }

    std::this_thread::sleep_for(100ms);
    SUCCEED();
}
