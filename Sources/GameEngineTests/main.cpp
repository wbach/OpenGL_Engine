//#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <SDL2/SDL.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "Logger/Log.h"

struct testStruct : public ::testing::Test
{
    bool a = true;
};

TEST_F(testStruct, dummyTest)
{
    EXPECT_TRUE(a);
}

int main(int argc, char** argv)
{
    CLogger::Instance().EnableLogs();
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    auto result = RUN_ALL_TESTS();
    return result;
}
