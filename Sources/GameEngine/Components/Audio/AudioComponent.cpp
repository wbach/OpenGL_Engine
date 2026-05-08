#include "AudioComponent.h"

#include <Logger/Log.h>
#include <Types.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <magic_enum/magic_enum.hpp>

#include "GameEngine/Audio/Attenuation.h"
#include "GameEngine/Audio/AudioId.h"
#include "GameEngine/Audio/IAudioManager.h"
#include "GameEngine/Audio/PlayGroup.h"
#include "GameEngine/Audio/PlayType.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_PLAY_TYPE[]   = "type";
constexpr char CSTR_GROUP[]       = "group";
constexpr char CSTR_ATTENUATION[] = "attenuation";

constexpr char CSTR_FILE[]         = "file";
constexpr char CSTR_VOLUME[]       = "volume";
constexpr char CSTR_PITCH[]        = "pitch";
constexpr char CSTR_MIN_DISTANCE[] = "minDistance";
constexpr char CSTR_MAX_DISTANCE[] = "maxDistance";

constexpr char CSTR_IS_SPATIAL[]     = "isSpatial";
constexpr char CSTR_IS_DIRECTIONAL[] = "isDirectional";
constexpr char CSTR_PLAY_ON_START[]  = "playOnStart";

constexpr char CSTR_INNER_ANGLE[] = "innerAngle";
constexpr char CSTR_OUTER_ANGLE[] = "outerAngle";
constexpr char CSTR_OUTER_GAIN[]  = "outerGain";
}  // namespace

REGISTER_COMPONENT(AudioComponent)

AudioComponent::AudioComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)

{
}

void AudioComponent::CleanUp()
{
    Stop();
}

void AudioComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, std::bind(&AudioComponent::Init, this));
    RegisterFunction(FunctionType::Update, std::bind(&AudioComponent::Update, this));
}

void AudioComponent::Reload()
{
    CleanUp();
    Init();
}

void AudioComponent::Init()
{
    if (playOnStart)
    {
        Play();
    }
}

PlayParameters AudioComponent::BuildParams() const
{
    PlayParameters params;
    params.volume   = volume;
    params.pitch    = pitch;
    params.playType = playType;

    if (isSpatial)
    {
        params.position    = thisObject_.GetWorldTransform().GetPosition();
        params.minDistance = minDistance;
        params.maxDistance = maxDistance;

        if (isDirectional)
        {
            auto rot         = thisObject_.GetWorldTransform().GetRotation();
            params.direction = rot.value_ * Direction(0.0f, 0.0f, 1.0f);
            params.cone      = SoundCone{innerAngle, outerAngle, outerGain};
        }
    }

    LOG_DEBUG << "Player params for : " << thisObject_.GetName() << ", file: " << file;
    LOG_DEBUG << params;

    return params;
}
void AudioComponent::Play()
{
    if (not IsActive())
        return;

    if (not file.exist())
    {
        LOG_WARN << "AudioComponent: Cannot play, file does not exist: " << file;
        return;
    }

    if (activeAudioId and activeAudioId != INVALID_AUDIO_ID)
    {
        Stop();
    }

    auto params   = BuildParams();
    activeAudioId = componentContext_.audioManager.play(file, playGroup, params);
}
void AudioComponent::Stop()
{
    if (activeAudioId and activeAudioId != INVALID_AUDIO_ID)
    {
        componentContext_.audioManager.stop(*activeAudioId);
    }
}
void AudioComponent::Update()
{
    if (not activeAudioId or activeAudioId == INVALID_AUDIO_ID or not isSpatial)
        return;

    auto transform = thisObject_.GetWorldTransform();
    componentContext_.audioManager.setSourcePosition(*activeAudioId, transform.GetPosition());

    if (isDirectional)
    {
        auto forward = transform.GetRotation().value_ * VECTOR_FORWARD;
        componentContext_.audioManager.setSourceDirection(*activeAudioId, forward);
    }
}
void AudioComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_FILE), file);
    ::Read(input.getChild(CSTR_PLAY_TYPE), playType);
    ::Read(input.getChild(CSTR_GROUP), playGroup);
    ::Read(input.getChild(CSTR_ATTENUATION), attenuation);
    ::Read(input.getChild(CSTR_MIN_DISTANCE), minDistance);
    ::Read(input.getChild(CSTR_MAX_DISTANCE), maxDistance);
    ::Read(input.getChild(CSTR_VOLUME), volume);
    ::Read(input.getChild(CSTR_PITCH), pitch);
    ::Read(input.getChild(CSTR_PLAY_ON_START), playOnStart);
    ::Read(input.getChild(CSTR_IS_SPATIAL), isSpatial);
    ::Read(input.getChild(CSTR_IS_DIRECTIONAL), isDirectional);
    ::Read(input.getChild(CSTR_INNER_ANGLE), innerAngle);
    ::Read(input.getChild(CSTR_OUTER_ANGLE), outerAngle);
    ::Read(input.getChild(CSTR_OUTER_GAIN), outerGain);
}
void AudioComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_PLAY_TYPE), playType);
    ::write(node.addChild(CSTR_GROUP), playGroup);
    ::write(node.addChild(CSTR_ATTENUATION), attenuation);
    ::write(node.addChild(CSTR_FILE), file.GetDataRelativePath());
    ::write(node.addChild(CSTR_VOLUME), volume);
    ::write(node.addChild(CSTR_PITCH), pitch);
    ::write(node.addChild(CSTR_PLAY_ON_START), playOnStart);
    ::write(node.addChild(CSTR_IS_SPATIAL), isSpatial);
    ::write(node.addChild(CSTR_MIN_DISTANCE), minDistance);
    ::write(node.addChild(CSTR_MAX_DISTANCE), maxDistance);
    ::write(node.addChild(CSTR_IS_DIRECTIONAL), isDirectional);
    ::write(node.addChild(CSTR_INNER_ANGLE), innerAngle);
    ::write(node.addChild(CSTR_OUTER_ANGLE), outerAngle);
    ::write(node.addChild(CSTR_OUTER_GAIN), outerGain);
}
}  // namespace Components
}  // namespace GameEngine
