#pragma once
#include <Utils/Time/Timer.h>
#include <string>
#include <unordered_map>
#include "SingleTon.h"
#include "Types.h"

namespace GameEngine
{
class Measurement
{
public:
    void Add(uint64 value);
    uint64 GetLastValue() const
    {
        return lastValue_;
    }
    uint64 GetMeanValue() const
    {
        return meanValue_;
    }

private:
    uint64 lastValue_;
    float meanValue_;
    uint64 count_;
};

class EngineMeasurement
{
public:
    void Add(const std::string& name, uint64 value);
    void WriteToFile(const std::string& fileName);

private:
    std::unordered_map<std::string, Measurement> measurements_;
    Utils::Timer timer_;
};
}  // GameEngine

#define MakeMeasurement(x, y)                                      \
    do                                                             \
    {                                                              \
        SingleTon<GameEngine::EngineMeasurement>::Get().Add(x, y); \
    } while (0)
#define WriteMeasurement(x)                                             \
    do                                                                  \
    {                                                                   \
        SingleTon<GameEngine::EngineMeasurement>::Get().WriteToFile(x); \
    } while (0)
