#include <SDL2/SDL_net.h>
#include <gtest/gtest.h>
#include "../Mocks/SDLNetWrapperMock.h"
#include "../UtilsNetwork/ServerCreator.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::NotNull;
using ::testing::Return;

namespace Network
{
class ServerCreatorTest : public ::testing::Test
{
public:
    ServerCreatorTest()
        : networkCreator_(sdlNetWrapperMock_)
    {
    }
    void SetUp()
    {
    }
    ~ServerCreatorTest()
    {
    }

protected:
    SDLNetWrapperMock sdlNetWrapperMock_;
    NetworkCreator networkCreator_;
};

TEST_F(ServerCreatorTest, InitPass)
{
    EXPECT_CALL(sdlNetWrapperMock_, Init()).Times(1).WillOnce(Return(1));
    EXPECT_TRUE(networkCreator_.Init());
}

TEST_F(ServerCreatorTest, InitFail)
{
    EXPECT_CALL(sdlNetWrapperMock_, GetError()).Times(1).WillOnce(Return(""));
    EXPECT_CALL(sdlNetWrapperMock_, Init()).Times(1).WillOnce(Return(-1));
    EXPECT_FALSE(networkCreator_.Init());
}

TEST_F(ServerCreatorTest, AllocSocketSetFail)
{
    EXPECT_CALL(sdlNetWrapperMock_, GetError()).Times(1).WillOnce(Return(""));
    EXPECT_CALL(sdlNetWrapperMock_, AllocSocketSet(_)).Times(1).WillOnce(Return(nullptr));
    EXPECT_FALSE(networkCreator_.AllocSocketSet(1));
}

}  // namespace Network
