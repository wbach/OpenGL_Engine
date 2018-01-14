#include <gtest/gtest.h>
#include "../UtilsNetwork/ServerCreator.h"
#include "../Mocks/Network/SDLNetWrapperMock.h"
#include <SDL2/SDL_net.h>

using ::testing::Return;
using ::testing::AtLeast;
using ::testing::_;
using ::testing::NotNull;

namespace Network
{
class ServerCreatorTest : public ::testing::Test
{
public:
	ServerCreatorTest()
		: sdlNetWrapperMock_(new SDLNetWrapperMock())
		, networkCreator_(sdlNetWrapperMock_)
	{}
	void SetUp()
	{	
	}
	~ServerCreatorTest()
	{
		//delete sdlNetWrapperMock_;
	}
protected:
	SDLNetWrapperMock* sdlNetWrapperMock_;
	CNetworkCreator networkCreator_;
};

TEST_F(ServerCreatorTest, InitPass)
{
	EXPECT_CALL(*sdlNetWrapperMock_, Init()).Times(1).WillOnce(Return(1));
	EXPECT_TRUE(networkCreator_.Init());
}

TEST_F(ServerCreatorTest, InitFail)
{
	EXPECT_CALL(*sdlNetWrapperMock_, GetError()).Times(1).WillOnce(Return(""));
	EXPECT_CALL(*sdlNetWrapperMock_, Init()).Times(1).WillOnce(Return(-1));
	EXPECT_FALSE(networkCreator_.Init());	
}

//TEST_F(ServerCreatorTest, AllocSocketSetPass)
//{
//	EXPECT_CALL(*sdlNetWrapperMock_, AllocSocketSet(_)).Times(1).WillOnce(Return(new SDLNet_SocketSet()));
//	EXPECT_TRUE(networkCreator_.AllocSocketSet());
//}

TEST_F(ServerCreatorTest, AllocSocketSetFail)
{
	EXPECT_CALL(*sdlNetWrapperMock_, GetError()).Times(1).WillOnce(Return(""));
	EXPECT_CALL(*sdlNetWrapperMock_, AllocSocketSet(_)).Times(1).WillOnce(Return(nullptr));
	EXPECT_FALSE(networkCreator_.AllocSocketSet(1));
}

} // Network
