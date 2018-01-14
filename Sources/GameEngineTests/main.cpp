//#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Logger/Log.h"
#include <SDL2/SDL.h>

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
	SDL_I
	CLogger::Instance().EnableLogs();
	::testing::InitGoogleTest(&argc, argv);
	::testing::InitGoogleMock(&argc, argv);
	auto result = RUN_ALL_TESTS();
	system("pause");
	return 0;
	
}