#include <SDL2/SDL.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "Logger/Log.h"

int main(int argc, char** argv)
{
    CLogger::Instance().EnableLogs();
    CLogger::Instance().ImmeditalyLog();
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    auto result = RUN_ALL_TESTS();
    return result;
}
