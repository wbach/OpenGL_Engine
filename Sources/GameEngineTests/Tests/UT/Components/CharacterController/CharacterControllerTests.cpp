#include "CharacterControllerTests.h"

CharacterControllerTests::CharacterControllerTests()
    : BaseComponentTestSchould()
    , sut_(context_, obj_)
{
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

    auto& clips                  = sut_.animationClipsNames_;
    clips.disarmed.idle          = "DI";
    clips.disarmed.sprint        = "DS";
    clips.disarmed.run.forward   = "DRF";
    clips.disarmed.run.backward  = "DRB";
    clips.disarmed.walk.forward  = "DWF";
    clips.disarmed.walk.backward = "DWB";
    clips.armed.idle             = "AI";
    clips.armed.sprint           = "AS";
    clips.armed.run.forward      = "ARF";
    clips.armed.run.backward     = "ARB";
    clips.armed.walk.forward     = "AWF";
    clips.armed.walk.backward    = "AWB";
    clips.equip                  = "equip";
    clips.disarm                 = "disarm";
    clips.disarmed.rotateLeft    = "DRL";
    clips.disarmed.rotateRight   = "DRR";
    clips.armed.rotateLeft       = "ARL";
    clips.armed.rotateRight      = "ARR";
    clips.drawArrow              = "drawArrow";
    clips.recoilArrow            = "recoilArrow";
    clips.aimIdle                = "aimIdle";
    clips.armed.death            = "armedDeath";
    clips.disarmed.death         = "disarmedDeath";

    addDummyClip(clips.equip);
    addDummyClip(clips.disarm);
    addDummyClip(clips.disarmed.death);
    addDummyClip(clips.armed.death);
    addDummyClip(clips.disarmed.idle);
    addDummyClip(clips.disarmed.sprint);
    addDummyClip(clips.disarmed.run.forward);
    addDummyClip(clips.disarmed.run.backward);
    addDummyClip(clips.disarmed.walk.forward);
    addDummyClip(clips.disarmed.walk.backward);
    addDummyClip(clips.armed.idle);
    addDummyClip(clips.armed.sprint);
    addDummyClip(clips.armed.run.forward);
    addDummyClip(clips.armed.run.backward);
    addDummyClip(clips.armed.walk.forward);
    addDummyClip(clips.armed.walk.backward);
    addDummyClip(clips.disarmed.rotateLeft);
    addDummyClip(clips.disarmed.rotateRight);
    addDummyClip(clips.armed.rotateLeft);
    addDummyClip(clips.armed.rotateRight);
    addDummyClip(clips.drawArrow);
    addDummyClip(clips.recoilArrow);
    addDummyClip(clips.aimIdle);

    sut_.equipTimeStamp  = DUMMY_CLIP_LENGTH;
    sut_.disarmTimeStamp = DUMMY_CLIP_LENGTH;

    auto weaponPtr =
            std::make_unique<GameObject>("WeaponObjName", componentController_, componentFactory_, IdType(1));
    weaponPtr->AddComponent<JointPoseUpdater>();
    obj_.AddChild(std::move(weaponPtr));

    sut_.Init();
    gpuResourceLoader_.RuntimeGpuTasks();
}

void CharacterControllerTests::SetUp()
{
    ASSERT_TRUE(sut_.fsm() != nullptr);
    EXPECT_CALL(physicsApiMock_, RemoveShape(shapeId));
    EXPECT_CALL(physicsApiMock_, RemoveRigidBody(rigidbodyid));

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

void CharacterControllerTests::addDummyClip(const std::string &name)
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
    DEBUG_LOG("Update");
    context_.time_.deltaTime = time;
    componentController_.CallFunctions(FunctionType::Update);
    sut_.Update();
}

void CharacterControllerTests::expectAnimsToBeSet(const std::vector<std::string> &names)
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

void CharacterControllerTests::expectForwardVelocity(float speed)
{
    DEBUG_LOG("Expected speed : " + std::to_string(speed));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, speed))).Times(AtLeast(1));
}

void CharacterControllerTests::expectRotatation(float deltaTime, float rotateSpeed)
{
    auto rotation = glm::angleAxis(glm::radians(rotateSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));

    DEBUG_LOG("Expected rotation : " + std::to_string(rotation));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(Quaternion(rotation))))
            .Times(AtLeast(1));
}

void CharacterControllerTests::expectRotationLeft(float dt)
{
    expectRotatation(dt, DEFAULT_TURN_SPEED);
}

void CharacterControllerTests::expectRotationRight(float dt)
{
    expectRotatation(dt, -DEFAULT_TURN_SPEED);
}
