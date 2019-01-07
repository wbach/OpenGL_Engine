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
    void Add(uint64_t value);
	uint64_t GetLastValue() const
    {
        return lastValue_;
    }
	uint64_t GetMeanValue() const
    {
        return static_cast<uint64_t>(meanValue_);
    }

private:
	uint64_t lastValue_;
    float meanValue_;
	uint64_t count_;
};

class EngineMeasurement
{
public:
    void Add(const std::string& name, uint64_t value);
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
