#pragma once
#include <optional>

#include "GameEngine/Audio/AudioId.h"
#include "GameEngine/Audio/PlayGroup.h"
#include "GameEngine/Audio/PlayParameters.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
namespace Components
{
class ENGINE_API AudioComponent : public BaseComponent
{
public:
    File file;
    PlayGroup playGroup{PlayGroup::SFX};
    PlayType playType{PlayType::Once};

    bool playOnStart{false};
    float volume{1.f};
    float pitch{1.f};

    Attenuation attenuation{Attenuation::Linear};
    bool isSpatial{false};
    float minDistance{1.0f};
    float maxDistance{100.0f};

    bool isDirectional{false};
    float innerAngle{360.0f};
    float outerAngle{360.0f};
    float outerGain{0.0f};

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_BOOL(playOnStart)
        FIELD_FILE(file)
        FIELD_ENUM(playGroup)
        FIELD_ENUM(playType)
        FIELD_FLOAT(volume)
        FIELD_FLOAT(pitch)
        FIELD_ENUM(attenuation)
        FIELD_BOOL(isSpatial)
        FIELD_FLOAT(minDistance)
        FIELD_FLOAT(maxDistance)
        FIELD_BOOL(isDirectional)
        FIELD_FLOAT(innerAngle)
        FIELD_FLOAT(outerAngle)
        FIELD_FLOAT(outerGain)
    END_FIELDS()
    // clang-format on

public:
    AudioComponent(ComponentContext&, GameObject&);

    void Init();
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void Play();
    void Stop();
    void Update();

private:
    PlayParameters BuildParams() const;

private:
    std::optional<AudioId> activeAudioId;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
