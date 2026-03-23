#include "AudioComponent.h"

#include <Utils/TreeNode.h>

#include <magic_enum/magic_enum.hpp>

#include "GameEngine/Audio/Attenuation.h"
#include "GameEngine/Audio/AudioId.h"
#include "GameEngine/Audio/IAudioManager.h"
#include "GameEngine/Audio/PlayGroup.h"
#include "GameEngine/Audio/PlayType.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "Logger/Log.h"
#include "Types.h"

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

AudioComponent::AudioComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<AudioComponent>(), componentContext, gameObject)

{
}

void AudioComponent::CleanUp()
{
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
void AudioComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<AudioComponent>(componentContext, gameObject);
        component->read(input);

        if (auto node = input.getChild(CSTR_PLAY_TYPE))
        {
            if (auto v = magic_enum::enum_cast<PlayType>(node->value_))
            {
                component->playType = *v;
            }
        }
        if (auto node = input.getChild(CSTR_GROUP))
        {
            if (auto v = magic_enum::enum_cast<PlayGroup>(node->value_))
            {
                component->playGroup = *v;
            }
        }
        if (auto node = input.getChild(CSTR_ATTENUATION))
        {
            if (auto v = magic_enum::enum_cast<Attenuation>(node->value_))
            {
                component->attenuation = *v;
            }
        }

        std::string fileStr;
        ::Read(input.getChild(CSTR_FILE), fileStr);
        component->file = fileStr;

        ::Read(input.getChild(CSTR_MIN_DISTANCE), component->minDistance);
        ::Read(input.getChild(CSTR_MAX_DISTANCE), component->maxDistance);
        ::Read(input.getChild(CSTR_VOLUME), component->volume);
        ::Read(input.getChild(CSTR_PITCH), component->pitch);
        ::Read(input.getChild(CSTR_PLAY_ON_START), component->playOnStart);
        ::Read(input.getChild(CSTR_IS_SPATIAL), component->isSpatial);
        ::Read(input.getChild(CSTR_IS_DIRECTIONAL), component->isDirectional);
        ::Read(input.getChild(CSTR_INNER_ANGLE), component->innerAngle);
        ::Read(input.getChild(CSTR_OUTER_ANGLE), component->outerAngle);
        ::Read(input.getChild(CSTR_OUTER_GAIN), component->outerGain);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<AudioComponent>(), readFunc);
}
void AudioComponent::write(TreeNode& node) const
{
    BaseComponent::write(node);

    ::write(node.addChild(CSTR_PLAY_TYPE), magic_enum::enum_name(playType));
    ::write(node.addChild(CSTR_GROUP), magic_enum::enum_name(playGroup));
    ::write(node.addChild(CSTR_ATTENUATION), magic_enum::enum_name(attenuation));
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
