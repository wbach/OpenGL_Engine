#include <Logger/Log.h>
#include <SDL2/SDL.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Utils.h"

int main(int argc, char** argv)
{
    auto args = Utils::parseArguments(argc, argv);

    if (Utils::GetValue(args, "verbose"))
    {
        CLogger::Instance().EnableLogs(LoggingLvl::ErrorWarningInfoDebug, CLogger::LogMode::SCREEN_ONLY);
        CLogger::Instance().UseAsyncLogging(false);
    }
    else
    {
        ::testing::GMOCK_FLAG(verbose) = "error";
    }
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    auto result = RUN_ALL_TESTS();
    return result;
}
