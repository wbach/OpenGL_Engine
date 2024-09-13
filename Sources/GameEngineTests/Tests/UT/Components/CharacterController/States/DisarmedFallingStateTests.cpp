#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.tiggerAndExpect<StartFallingEvent>({test.sut_.animationClipsNames_.aim.draw});
}
}  // namespace
