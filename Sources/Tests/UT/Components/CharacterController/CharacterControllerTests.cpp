#include "CharacterControllerTests.h"

#include <GameEngine/Components/Animation/JointPoseUpdater.h>
#include <GameEngine/Physics/IPhysicsApi.h>
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Animation/BowPoseUpdater.h"
#include <Logger/Log.h>

MATCHER_P(CollisionDetectionActionMatcher, action, "Action matcher for CollisionDetection")
{
    return (arg.action == action);
}

CharacterControllerTests::CharacterControllerTests()
    : BaseComponentTestSchould()
    , sut_(context_, *obj_)
{
    LOG_DEBUG << "CharacterControllerTests::CharacterControllerTests";

    EXPECT_CALL(physicsApiMock_, SetAngularFactor(_, Matcher<float>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, CreateSphereColider(_, _, _)).WillOnce(Return(shapeId));
    EXPECT_CALL(physicsApiMock_, CreateRigidbody(shapeId, _, _, _, _)).WillOnce(Return(rigidbodyid));

    EXPECT_CALL(physicsApiMock_,
                setCollisionCallback(_, CollisionDetectionActionMatcher(GameEngine::Physics::CollisionDetection::Action::onExit)))
        .WillOnce(DoAll(SaveArg<1>(&groundExitSub.second), Return(groundExitSub.first)));

    EXPECT_CALL(
        physicsApiMock_,
        setCollisionCallback(_, CollisionDetectionActionMatcher(GameEngine::Physics::CollisionDetection::Action::onEnter)))
        .WillOnce(DoAll(SaveArg<1>(&groundEnterSub.second), Return(groundEnterSub.first)));

    animator_  = &obj_->AddComponent<Animator>();
    rigidbody_ = &obj_->AddComponent<Rigidbody>();

    obj_->AddComponent<RendererComponent>().AddModel(&model_);
    obj_->AddComponent<SphereShape>();

    createDummySkeleton();
    initAnimations();

    auto weaponPtr =
        std::make_unique<GameObject>("WeaponObjName", componentController_, componentFactory_, gameObjectIdPool, IdType(1));
    weaponPtr->AddComponent<BowPoseUpdater>();
    scene.AddGameObject(*obj_, std::move(weaponPtr));

    sut_.ReqisterFunctions();

    componentController_.CallFunctions(FunctionType::Awake);
    componentController_.CallFunctions(FunctionType::LateAwake);
    componentController_.CallFunctions(FunctionType::OnStart);
    componentController_.CallFunctions(FunctionType::PostStart);

    sut_.Init();
    gpuResourceLoader_.RuntimeGpuTasks();
}
CharacterControllerTests::~CharacterControllerTests()
{
    EXPECT_CALL(physicsApiMock_, RemoveShape(shapeId));
    EXPECT_CALL(physicsApiMock_, RemoveRigidBody(rigidbodyid));
    LOG_DEBUG << "";
}
void CharacterControllerTests::SetUp()
{
    EXPECT_CALL(inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));

    Update(ADVANCED_TIME_TRANSITION_TIME);  // To set first anim
}

void CharacterControllerTests::TearDown()
{
}

void CharacterControllerTests::initAnimations()
{
    auto& clips  = sut_.animationClipsNames_;
    clips.equip  = "equip";
    clips.disarm = "disarm";

    clips.aim.idle   = "aimIdle";
    clips.aim.draw   = "drawArrow";
    clips.aim.recoil = "recoilArrow";

    clips.disarmed.posture.crouched.idle         = "DCI";
    clips.disarmed.posture.crouched.death        = "DCD";
    clips.disarmed.posture.crouched.hurt         = "DCH";
    clips.disarmed.posture.crouched.rotate.left  = "DCRL";
    clips.disarmed.posture.crouched.rotate.right = "DCRR";

    clips.disarmed.posture.stand.idle         = "DI";
    clips.disarmed.posture.stand.death        = "DD";
    clips.disarmed.posture.stand.hurt         = "DH";
    clips.disarmed.posture.stand.rotate.left  = "DRL";
    clips.disarmed.posture.stand.rotate.right = "DRR";

    clips.disarmed.movement.run.forward  = "DRF";
    clips.disarmed.movement.run.backward = "DRB";
    clips.disarmed.movement.run.left     = "DMRL";
    clips.disarmed.movement.run.right    = "DMRR";

    clips.disarmed.movement.walk.forward  = "DWF";
    clips.disarmed.movement.walk.backward = "DWB";
    clips.disarmed.movement.walk.left     = "DMWL";
    clips.disarmed.movement.walk.right    = "DMWR";

    clips.disarmed.movement.crouch.forward  = "DCWF";
    clips.disarmed.movement.crouch.backward = "DCWB";
    clips.disarmed.movement.crouch.left     = "DCMRL";
    clips.disarmed.movement.crouch.right    = "DCMRR";

    clips.disarmed.sprint         = "DS";
    clips.disarmed.jump           = "DJ";
    clips.disarmed.falling        = "DF";
    clips.disarmed.dodgeDive      = "DDD";
    clips.disarmed.dodge.forward  = "DDF";
    clips.disarmed.dodge.backward = "DDB";
    clips.disarmed.dodge.left     = "DDL";
    clips.disarmed.dodge.right    = "DDR";

    clips.disarmed.attack.push_back({"DA1", PlayStateType::idle});
    clips.disarmed.attack.push_back({"DA2", PlayStateType::idle});
    clips.disarmed.attack.push_back({"DA3", PlayStateType::idle});

    disarmedAttackClip1 = &clips.disarmed.attack[0];
    disarmedAttackClip2 = &clips.disarmed.attack[1];
    disarmedAttackClip3 = &clips.disarmed.attack[2];

    clips.armed.posture.crouched.idle         = "ACI";
    clips.armed.posture.crouched.death        = "ACD";
    clips.armed.posture.crouched.hurt         = "ACH";
    clips.armed.posture.crouched.rotate.left  = "ACRL";
    clips.armed.posture.crouched.rotate.right = "ACRR";

    clips.armed.posture.stand.idle         = "AI";
    clips.armed.posture.stand.death        = "AD";
    clips.armed.posture.stand.hurt         = "AH";
    clips.armed.posture.stand.rotate.left  = "ARL";
    clips.armed.posture.stand.rotate.right = "ARR";

    clips.armed.movement.run.forward  = "ARF";
    clips.armed.movement.run.backward = "ARB";
    clips.armed.movement.run.left     = "AMRL";
    clips.armed.movement.run.right    = "AMRR";

    clips.armed.movement.walk.forward  = "AWF";
    clips.armed.movement.walk.backward = "AWB";
    clips.armed.movement.walk.left     = "AMWL";
    clips.armed.movement.walk.right    = "AMWR";

    clips.armed.movement.crouch.forward  = "ACWF";
    clips.armed.movement.crouch.backward = "ACWB";
    clips.armed.movement.crouch.left     = "ACMRL";
    clips.armed.movement.crouch.right    = "ACMRR";

    clips.armed.sprint         = "AS";
    clips.armed.jump           = "AJ";
    clips.armed.falling        = "AF";
    clips.armed.dodgeDive      = "ADD";
    clips.armed.dodge.forward  = "ADF";
    clips.armed.dodge.backward = "ADB";
    clips.armed.dodge.left     = "ADL";
    clips.armed.dodge.right    = "ADR";
    clips.armed.attack.push_back({"A1", PlayStateType::idle});
    clips.armed.attack.push_back({"A2", PlayStateType::idle});
    clips.armed.attack.push_back({"A3", PlayStateType::idle});

    armedAttackClip1 = &clips.armed.attack[0];
    armedAttackClip2 = &clips.armed.attack[1];
    armedAttackClip3 = &clips.armed.attack[2];

    addDummyClip(clips.equip);
    addDummyClip(clips.disarm);
    addDummyClip(clips.aim.idle);
    addDummyClip(clips.aim.draw);
    addDummyClip(clips.aim.recoil);

    for (const auto& attackClip : clips.disarmed.attack)
    {
        addDummyClip(attackClip.name);
    }

    addDummyClip(clips.disarmed.jump);
    addDummyClip(clips.disarmed.falling);
    addDummyClip(clips.disarmed.sprint);
    addDummyClip(clips.disarmed.dodgeDive);
    addDummyClip(clips.disarmed.dodge.forward);
    addDummyClip(clips.disarmed.dodge.backward);
    addDummyClip(clips.disarmed.dodge.left);
    addDummyClip(clips.disarmed.dodge.right);

    addDummyClip(clips.disarmed.posture.crouched.death);
    addDummyClip(clips.disarmed.posture.crouched.idle);
    addDummyClip(clips.disarmed.posture.crouched.hurt);
    addDummyClip(clips.disarmed.posture.crouched.rotate.left);
    addDummyClip(clips.disarmed.posture.crouched.rotate.right);

    addDummyClip(clips.disarmed.posture.stand.death);
    addDummyClip(clips.disarmed.posture.stand.idle);
    addDummyClip(clips.disarmed.posture.stand.hurt);
    addDummyClip(clips.disarmed.posture.stand.rotate.left);
    addDummyClip(clips.disarmed.posture.stand.rotate.right);

    addDummyClip(clips.disarmed.movement.run.forward);
    addDummyClip(clips.disarmed.movement.run.backward);
    addDummyClip(clips.disarmed.movement.run.left);
    addDummyClip(clips.disarmed.movement.run.right);

    addDummyClip(clips.disarmed.movement.walk.forward);
    addDummyClip(clips.disarmed.movement.walk.backward);
    addDummyClip(clips.disarmed.movement.walk.left);
    addDummyClip(clips.disarmed.movement.walk.right);

    addDummyClip(clips.disarmed.movement.crouch.forward);
    addDummyClip(clips.disarmed.movement.crouch.backward);
    addDummyClip(clips.disarmed.movement.crouch.left);
    addDummyClip(clips.disarmed.movement.crouch.right);

    addDummyClip(clips.armed.posture.crouched.death);
    addDummyClip(clips.armed.posture.crouched.idle);
    addDummyClip(clips.armed.posture.crouched.hurt);
    addDummyClip(clips.armed.posture.crouched.rotate.left);
    addDummyClip(clips.armed.posture.crouched.rotate.right);

    addDummyClip(clips.armed.posture.stand.death);
    addDummyClip(clips.armed.posture.stand.idle);
    addDummyClip(clips.armed.posture.stand.hurt);
    addDummyClip(clips.armed.posture.stand.rotate.left);
    addDummyClip(clips.armed.posture.stand.rotate.right);

    addDummyClip(clips.armed.movement.run.forward);
    addDummyClip(clips.armed.movement.run.backward);
    addDummyClip(clips.armed.movement.run.left);
    addDummyClip(clips.armed.movement.run.right);

    addDummyClip(clips.armed.movement.walk.forward);
    addDummyClip(clips.armed.movement.walk.backward);
    addDummyClip(clips.armed.movement.walk.left);
    addDummyClip(clips.armed.movement.walk.right);

    addDummyClip(clips.armed.movement.crouch.forward);
    addDummyClip(clips.armed.movement.crouch.backward);
    addDummyClip(clips.armed.movement.crouch.left);
    addDummyClip(clips.armed.movement.crouch.right);

    addDummyClip(clips.armed.jump);
    addDummyClip(clips.armed.sprint);
    addDummyClip(clips.armed.falling);
    addDummyClip(clips.armed.dodgeDive);
    addDummyClip(clips.armed.dodge.forward);
    addDummyClip(clips.armed.dodge.backward);
    addDummyClip(clips.armed.dodge.left);
    addDummyClip(clips.armed.dodge.right);

    for (const auto& attackClip : clips.armed.attack)
    {
        addDummyClip(attackClip.name);
    }

    sut_.equipTimeStamp  = DUMMY_CLIP_LENGTH;
    sut_.disarmTimeStamp = DUMMY_CLIP_LENGTH;
}

void CharacterControllerTests::createDummySkeleton()
{
    animator_->jointGroups_.insert({sut_.lowerBodyGroupName, {"head", "neck", "leftHand", "rightHand"}});
    animator_->jointGroups_.insert({sut_.upperBodyGroupName, {"hips, leftLeg", "rightLeg"}});

    Animation::JointId id = 0;
    Animation::Joint rootJoint;
    rootJoint.name = "Armature";
    rootJoint.id   = id;

    for (const auto& [_, group] : animator_->jointGroups_)
    {
        for (const auto& jointName : group)
        {
            Animation::Joint joint;
            joint.name = jointName;
            joint.id   = ++id;
            rootJoint.addChild(joint);
        }
    }

    model_.setRootJoint(rootJoint);
}

void CharacterControllerTests::addDummyClip(const std::string& name)
{
    Animation::AnimationClip clip(name);
    for (int i = 0; i <= DUMMY_FRAMES; ++i)
    {
        clip.AddFrame(Animation::KeyFrame{{DUMMY_FRAME_TIME_DELTA * (float)i}, {{0, Animation::JointTransform{}}}});
    }
    LOG_DEBUG << "addDummyClip : " << name << " Length : " << clip.GetLength();
    animator_->AddAnimationClip(name, clip);
}

void CharacterControllerTests::Update(float time)
{
    LOG_DEBUG << "Update deltaTime: " << time;
    context_.time_.deltaTime = time;
    componentController_.CallFunctions(FunctionType::Update);
}

void CharacterControllerTests::expectAnyRotation()
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
}

void CharacterControllerTests::expectAnimsToBeSet(const std::vector<std::string>& names)
{
    EXPECT_EQ(animator_->getCurrentAnimationName().size(), names.size());

    for (const auto& name : names)
    {
        LOG_DEBUG << "Expected : " << name;
    }

    for (const auto& name : animator_->getCurrentAnimationName())
    {
        LOG_DEBUG << "Current  : " << name;
        auto iter = std::find(names.begin(), names.end(), name);
        EXPECT_TRUE(iter != names.end());
    }
}

void CharacterControllerTests::expectNoMove()
{
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, _)).Times(0);
}

void CharacterControllerTests::expectNoRotation()
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(0);
}

void CharacterControllerTests::expectVelocity(const vec3& dir, const vec3& moveSpeed, const vec3& currentVelocity)
{
    auto normalizedDir = glm::normalize(dir);
    auto velocity      = normalizedDir * glm::length(moveSpeed * normalizedDir);
    LOG_DEBUG << "Expected dir : " << dir;
    LOG_DEBUG << "Expected speed : " << moveSpeed;
    LOG_DEBUG << "Expected velocity : " << velocity;
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(currentVelocity));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, velocity))
        .Times(AtLeast(1))
        .WillRepeatedly(SaveArg<1>(&lastSetVelocity));
}

void CharacterControllerTests::expectForwardVelocity(float speed)
{
    LOG_DEBUG << "Expected speed : " << speed;
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, speed)))
        .Times(AtLeast(1))
        .WillRepeatedly(SaveArg<1>(&lastSetVelocity));
}

void CharacterControllerTests::expectLeftVelocity(float speed)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(speed, 0.0, 0.0)))
        .Times(AtLeast(1))
        .WillRepeatedly(SaveArg<1>(&lastSetVelocity));
}

Rotation CharacterControllerTests::createRotaion(float deltaTime, float rotateSpeed)
{
    return glm::angleAxis(glm::radians(rotateSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));
}

void CharacterControllerTests::expectRotatation(float deltaTime, float rotateSpeed)
{
    auto rotation = createRotaion(rotateSpeed, deltaTime);

    /* LOG TO FIX*/ LOG_ERROR << ("Expected rotation : " + std::to_string(rotation.value_));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(rotation.value_))).Times(AtLeast(1));
}

void CharacterControllerTests::expectRotationLeft(float dt)
{
    expectRotatation(dt, DEFAULT_TURN_SPEED);
}

void CharacterControllerTests::expectRotationRight(float dt)
{
    expectRotatation(dt, -DEFAULT_TURN_SPEED);
}

void CharacterControllerTests::expectRootboneRotation(const vec3& dir)
{
    Update(DEFAULT_MOVING_CHANGE_DIR_SPEED);

    auto currentBoneRotation  = glm::quat_cast(animator_->GetRootJoint()->additionalUserMofiyTransform.getMatrix());
    auto expectedBoneRotation = glm::angleAxis(glm::orientedAngle(VECTOR_FORWARD, glm::normalize(dir), VECTOR_UP), VECTOR_UP);

    EXPECT_NEAR(currentBoneRotation.x, expectedBoneRotation.x, std::numeric_limits<float>::epsilon());
    EXPECT_NEAR(currentBoneRotation.y, expectedBoneRotation.y, std::numeric_limits<float>::epsilon());
    EXPECT_NEAR(currentBoneRotation.z, expectedBoneRotation.z, std::numeric_limits<float>::epsilon());
    EXPECT_NEAR(currentBoneRotation.w, expectedBoneRotation.w, std::numeric_limits<float>::epsilon());

    /* LOG TO FIX*/ LOG_ERROR << ("Expected bone rotation : " + std::to_string(expectedBoneRotation) + ", eurler " +
                                  std::to_string(Rotation(expectedBoneRotation).GetEulerDegrees().value));
    /* LOG TO FIX*/ LOG_ERROR << ("Current bone rotation : " + std::to_string(currentBoneRotation) + ", eurler " +
                                  std::to_string(Rotation(currentBoneRotation).GetEulerDegrees().value));
}
