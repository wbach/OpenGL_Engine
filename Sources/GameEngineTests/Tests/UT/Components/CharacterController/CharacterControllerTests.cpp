#include "CharacterControllerTests.h"

CharacterControllerTests::CharacterControllerTests()
    : BaseComponentTestSchould()
    , sut_(context_, obj_)
{
    CLogger::Instance().EnableLogs();
    CLogger::Instance().ImmeditalyLog();

    DEBUG_LOG("CharacterControllerTests::CharacterControllerTests");

    EXPECT_CALL(physicsApiMock_, SetAngularFactor(_, Matcher<float>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, CreateSphereColider(_, _, _)).WillOnce(Return(shapeId));
    EXPECT_CALL(physicsApiMock_, CreateRigidbody(shapeId, _, _, _, _)).WillOnce(Return(rigidbodyid));

    animator_  = &obj_.AddComponent<Animator>();
    rigidbody_ = &obj_.AddComponent<Rigidbody>();

    obj_.AddComponent<RendererComponent>().AddModel(&model_);
    obj_.AddComponent<SphereShape>();

    createDummySkeleton();

    for (auto& component : obj_.GetComponents())
    {
        component->ReqisterFunctions();
    }

    sut_.ReqisterFunctions();

    componentController_.CallFunctions(FunctionType::Awake);
    componentController_.CallFunctions(FunctionType::OnStart);

    auto& clips       = sut_.animationClipsNames_;
    clips.equip       = "equip";
    clips.disarm      = "disarm";
    clips.aimIdle     = "aimIdle";
    clips.drawArrow   = "drawArrow";
    clips.recoilArrow = "recoilArrow";

    clips.disarmed.idle             = "DI";
    clips.disarmed.crouchIdle       = "DCI";
    clips.disarmed.sprint           = "DS";
    clips.disarmed.run.forward      = "DRF";
    clips.disarmed.run.backward     = "DRB";
    clips.disarmed.run.moveleft     = "DMRL";
    clips.disarmed.run.moveRight    = "DMRR";
    clips.disarmed.walk.forward     = "DWF";
    clips.disarmed.walk.backward    = "DWB";
    clips.disarmed.walk.moveleft    = "DMRL";
    clips.disarmed.walk.moveRight   = "DMRR";
    clips.disarmed.crouch.forward   = "DCWF";
    clips.disarmed.crouch.backward  = "DCWB";
    clips.disarmed.crouch.moveleft  = "DCMRL";
    clips.disarmed.crouch.moveRight = "DCMRR";
    clips.disarmed.rotateLeft       = "DRL";
    clips.disarmed.rotateRight      = "DRR";
    clips.disarmed.death            = "disarmedDeath";
    clips.disarmed.attack.push_back({"DA1", PlayStateType::idle});
    clips.disarmed.attack.push_back({"DA2", PlayStateType::idle});
    clips.disarmed.attack.push_back({"DA3", PlayStateType::idle});
    disarmedAttackClip1 = &clips.disarmed.attack[0];
    disarmedAttackClip2 = &clips.disarmed.attack[1];
    disarmedAttackClip3 = &clips.disarmed.attack[2];

    clips.armed.idle             = "AI";
    clips.armed.crouchIdle       = "ACI";
    clips.armed.sprint           = "AS";
    clips.armed.run.forward      = "ARF";
    clips.armed.run.backward     = "ARB";
    clips.armed.run.moveleft     = "AMRL";
    clips.armed.run.moveRight    = "AMRR";
    clips.armed.walk.forward     = "AWF";
    clips.armed.walk.backward    = "AWB";
    clips.armed.walk.moveleft    = "AMRL";
    clips.armed.walk.moveRight   = "AMRR";
    clips.armed.crouch.forward   = "ACWF";
    clips.armed.crouch.backward  = "ACWB";
    clips.armed.crouch.moveleft  = "ACMRL";
    clips.armed.crouch.moveRight = "ACMRR";
    clips.armed.rotateLeft       = "ARL";
    clips.armed.rotateRight      = "ARR";
    clips.armed.death            = "armedDeath";
    clips.armed.attack.push_back({"A1", PlayStateType::idle});
    clips.armed.attack.push_back({"A2", PlayStateType::idle});
    clips.armed.attack.push_back({"A3", PlayStateType::idle});

    armedAttackClip1 = &clips.armed.attack[0];
    armedAttackClip2 = &clips.armed.attack[1];
    armedAttackClip3 = &clips.armed.attack[2];

    addDummyClip(clips.equip);
    addDummyClip(clips.disarm);
    addDummyClip(clips.aimIdle);
    addDummyClip(clips.drawArrow);
    addDummyClip(clips.recoilArrow);

    addDummyClip(clips.disarmed.death);
    addDummyClip(clips.disarmed.idle);
    addDummyClip(clips.disarmed.crouchIdle);
    addDummyClip(clips.disarmed.sprint);
    addDummyClip(clips.disarmed.run.forward);
    addDummyClip(clips.disarmed.run.backward);
    addDummyClip(clips.disarmed.run.moveleft);
    addDummyClip(clips.disarmed.run.moveRight);
    addDummyClip(clips.disarmed.walk.forward);
    addDummyClip(clips.disarmed.walk.backward);
    addDummyClip(clips.disarmed.walk.moveleft);
    addDummyClip(clips.disarmed.walk.moveRight);
    addDummyClip(clips.disarmed.crouch.forward);
    addDummyClip(clips.disarmed.crouch.backward);
    addDummyClip(clips.disarmed.crouch.moveleft);
    addDummyClip(clips.disarmed.crouch.moveRight);
    addDummyClip(clips.disarmed.rotateLeft);
    addDummyClip(clips.disarmed.rotateRight);

    addDummyClip(clips.armed.idle);
    addDummyClip(clips.armed.death);
    addDummyClip(clips.armed.crouchIdle);
    addDummyClip(clips.armed.sprint);
    addDummyClip(clips.armed.run.forward);
    addDummyClip(clips.armed.run.backward);
    addDummyClip(clips.armed.run.moveleft);
    addDummyClip(clips.armed.run.moveRight);
    addDummyClip(clips.armed.walk.forward);
    addDummyClip(clips.armed.walk.backward);
    addDummyClip(clips.armed.walk.moveleft);
    addDummyClip(clips.armed.walk.moveRight);
    addDummyClip(clips.armed.crouch.forward);
    addDummyClip(clips.armed.crouch.backward);
    addDummyClip(clips.armed.crouch.moveleft);
    addDummyClip(clips.armed.crouch.moveRight);
    addDummyClip(clips.armed.rotateLeft);
    addDummyClip(clips.armed.rotateRight);

    for (const auto& attackClip : clips.disarmed.attack)
    {
        addDummyClip(attackClip.name);
    }
    for (const auto& attackClip : clips.armed.attack)
    {
        addDummyClip(attackClip.name);
    }

    sut_.equipTimeStamp  = DUMMY_CLIP_LENGTH;
    sut_.disarmTimeStamp = DUMMY_CLIP_LENGTH;

    auto weaponPtr = std::make_unique<GameObject>("WeaponObjName", componentController_, componentFactory_, IdType(1));
    weaponPtr->AddComponent<BowPoseUpdater>();
    obj_.AddChild(std::move(weaponPtr));

    sut_.Init();
    gpuResourceLoader_.RuntimeGpuTasks();
}

void CharacterControllerTests::SetUp()
{
    EXPECT_CALL(physicsApiMock_, RemoveShape(shapeId));
    EXPECT_CALL(physicsApiMock_, RemoveRigidBody(rigidbodyid));
    EXPECT_CALL(inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));

    Update(ADVANCED_TIME_TRANSITION_TIME);  // To set first anim
}

void CharacterControllerTests::TearDown()
{
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
        clip.AddFrame(Animation::KeyFrame{DUMMY_FRAME_TIME_DELTA * (float)i, {{0, Animation::JointTransform{}}}});
    }
    DEBUG_LOG("addDummyClip : " + name + " Length : " + std::to_string(clip.GetLength()));
    animator_->AddAnimationClip(clip);
}

void CharacterControllerTests::Update(float time)
{
    DEBUG_LOG("Update deltaTime: " + std::to_string(time));
    context_.time_.deltaTime = time;
    componentController_.CallFunctions(FunctionType::Update);
    sut_.Update();
}

void CharacterControllerTests::expectAnimsToBeSet(const std::vector<std::string>& names)
{
    EXPECT_EQ(animator_->getCurrentAnimationName().size(), names.size());

    for (const auto& name : names)
    {
        DEBUG_LOG("Expected : " + name);
    }

    for (const auto& name : animator_->getCurrentAnimationName())
    {
        DEBUG_LOG("Current  : " + name);
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

void CharacterControllerTests::expectVelocity(const vec3& dir, const vec3& moveSpeed)
{
    auto normalizedDir = glm::normalize(dir);
    auto velocity      = normalizedDir * glm::length(moveSpeed * normalizedDir);
    DEBUG_LOG("Expected dir : " + std::to_string(dir));
    DEBUG_LOG("Expected speed : " + std::to_string(moveSpeed));
    DEBUG_LOG("Expected velocity : " + std::to_string(velocity));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, velocity)).Times(AtLeast(1));
}

void CharacterControllerTests::expectForwardVelocity(float speed)
{
    DEBUG_LOG("Expected speed : " + std::to_string(speed));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, speed))).Times(AtLeast(1));
}

void CharacterControllerTests::expectLeftVelocity(float speed)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(speed, 0.0, 0.0))).Times(AtLeast(1));
}

Rotation CharacterControllerTests::createRotaion(float deltaTime, float rotateSpeed)
{
    return glm::angleAxis(glm::radians(rotateSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));
}

void CharacterControllerTests::expectRotatation(float deltaTime, float rotateSpeed)
{
    auto rotation = createRotaion(rotateSpeed, deltaTime);

    DEBUG_LOG("Expected rotation : " + std::to_string(rotation.value_));
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

    DEBUG_LOG("Expected bone rotation : " + std::to_string(expectedBoneRotation) + ", eurler " +
              std::to_string(Rotation(expectedBoneRotation).GetEulerDegrees().value));
    DEBUG_LOG("Current bone rotation : " + std::to_string(currentBoneRotation) + ", eurler " +
              std::to_string(Rotation(currentBoneRotation).GetEulerDegrees().value));
}
