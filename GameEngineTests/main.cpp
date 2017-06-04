#include <gtest/gtest.h>

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
	::testing::InitGoogleTest(&argc, argv);
	auto result = RUN_ALL_TESTS();
	system("pause");
	return 0;
	
}