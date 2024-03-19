//#include "CharacterControllerFsmTests.h"

//namespace
//{
//AnimationClipsNames createClipNames()
//{
//    AnimationClipsNames clips;
//    clips.disarmed.idle           = "DI";
//    clips.disarmed.sprint         = "DS";
//    clips.disarmed.run.forward    = "DRF";
//    clips.disarmed.run.backward   = "DRB";
//    clips.disarmed.walk.forward   = "DWF";
//    clips.disarmed.walk.backward  = "DWB";
//    clips.armed.idle              = "AI";
//    clips.armed.sprint            = "AS";
//    clips.armed.run.forward       = "ARF";
//    clips.armed.run.backward      = "ARB";
//    clips.armed.walk.forward      = "AWF";
//    clips.armed.walk.backward     = "AWB";
//    clips.equip                   = "equip";
//    clips.disarm                  = "disarm";
//    clips.disarmed.rotateLeft     = "DRL";
//    clips.disarmed.rotateRight    = "DRR";
//    clips.armed.rotateLeft        = "ARL";
//    clips.armed.rotateRight       = "ARR";
//    clips.drawArrow               = "drawArrow";
//    clips.recoilArrow             = "recoilArrow";
//    clips.aimIdle                 = "aimIdle";
//    clips.armed.death             = "armedDeath";
//    clips.disarmed.death          = "disarmedDeath";
//    clips.disarmed.run.moveleft   = "DMRL";
//    clips.disarmed.run.moveRight  = "DMRR";
//    clips.disarmed.walk.moveleft  = "DMRL";
//    clips.disarmed.walk.moveRight = "DMRR";
//    clips.armed.run.moveleft      = "DMRL";
//    clips.armed.run.moveRight     = "DMRR";
//    clips.armed.walk.moveleft     = "DMRL";
//    clips.armed.walk.moveRight    = "DMRR";
//    return clips;
//}
//const float equipTimeStamp = 0.0f;
//const float disarmTimeStamp = 1.f;
//const std::string lowerBodyGroupName{"lowerBodyGroupName"};
//const std::string upperBodyGroupName{"upperBodyGroupName"};
//}  // namespace
//CharacterControllerFsmTests::CharacterControllerFsmTests()
//    : animationClipsNames(createClipNames())
//    , fsmContext(gameObject, physicsApiMock, *rigidbody_, *animator_, *this, inputManagerMock, *aimController_, {}, {},
//                 animationClipsNames, upperBodyGroupName, lowerBodyGroupName, {equipTimeStamp, disarmTimeStamp} )
//    , sut(fsmContext)
//{
//}
