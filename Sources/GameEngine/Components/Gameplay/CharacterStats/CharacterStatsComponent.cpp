#include "CharacterStatsComponent.h"

#include <Input/InputManager.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <string>
#include <utility>

#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/GUI/Button/Button.h"
#include "GameEngine/Renderers/GUI/ElementReader.h"
#include "GameEngine/Renderers/GUI/ElementWriter.h"
#include "GameEngine/Renderers/GUI/IElementFactory.h"
#include "GameEngine/Renderers/GUI/IGuiRenderer.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Manager.h"
#include "GameEngine/Renderers/GUI/Text/MultiLineText.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "GameEngine/Renderers/GUI/Transform.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace CharacterParam
{
constexpr char GuildName[]        = "GUILD_NAME";
constexpr char LvlValue[]         = "LVL_VALUE";
constexpr char ExpValue[]         = "EXP_VALUE";
constexpr char NextLvlValue[]     = "NEXT_LVL_VALUE";
constexpr char SkillPointsValue[] = "SKILL_POINTS_VALUE";
constexpr char StrValue[]         = "STR_VALUE";
constexpr char DexValue[]         = "DEX_VALUE";
constexpr char ManaValue[]        = "MANA_VALUE";
constexpr char HpValue[]          = "HP_VALUE";
constexpr char RWeapon[]          = "R_WEAPON";
constexpr char RArrow[]           = "R_ARROW";
constexpr char RFire[]            = "R_FIRE";
constexpr char RMagic[]           = "R_MAGIC";
constexpr char OneHandValue[]     = "ONE_HAND_VALUE";
constexpr char TwoHandValue[]     = "TWO_HAND_VALUE";
constexpr char BowValue[]         = "BOW_VALUE";
constexpr char CrossbowValue[]    = "CROSSBOW_VALUE";
constexpr char MagicValue[]       = "MAGIC_VALUE";
constexpr char OpenLocks[]        = "OPEN_LOCKS";
constexpr char PickPocketing[]    = "PICK_POCKETING";
constexpr char SneakingValue[]    = "SNEAKING_VALUE";
constexpr char AcrobaticsValue[]  = "ACROBATICS_VALUE";
}  // namespace CharacterParam
namespace
{
constexpr char GUI_FILE[]{"guiLayoutFile"};

}  // namespace
namespace Components
{
CharacterStatsComponent::CharacterStatsComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<CharacterStatsComponent>(), componentContext, gameObject)
{
}
CharacterStatsComponent::~CharacterStatsComponent()
{
}
void CharacterStatsComponent::CleanUp()
{
}
void CharacterStatsComponent::Reload()
{
}
void CharacterStatsComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, [this]() { initGui(); });

    RegisterFunction(FunctionType::OnStart,
                     [this]()
                     {
                         componentContext_.inputManager_.SubscribeOnKeyDown(Input::GameAction::CHARACTER_VIEW,
                                                                            [this]()
                                                                            {
                                                                                if (not mainWindow)
                                                                                    return;

                                                                                if (mainWindow->isActive())
                                                                                {
                                                                                    hide();
                                                                                }
                                                                                else
                                                                                {
                                                                                    show();
                                                                                }
                                                                            });
                     });
}
void CharacterStatsComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<CharacterStatsComponent>(componentContext, gameObject);
        component->read(input);

        ::Read(input.getChild(GUI_FILE), component->guiFile);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<CharacterStatsComponent>(), func);
}
void CharacterStatsComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(GUI_FILE), guiFile);
}
void CharacterStatsComponent::initGui()
{
    if (mainWindow)
    {
        LOG_DEBUG << "Already initialized";
        return;
    }

    const std::string layerName{"CharacterViewer"};
    GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);

    if (reader.read(guiFile, layerName))
    {
        auto layer = componentContext_.guiManager_.getLayer(layerName);

        mainWindow = GUI::getTypedElement<GUI::Window>(layer, "MainWindow");

        if (auto exitButton = GUI::getTypedElement<GUI::Button>(layer, "Exit"))
        {
            exitButton->setOnClick([this]() { hide(); });
        }

        if (mainWindow)
        {
            mainWindow->activate(false);
        }

        auto getParamTxt = [&](const std::string& label)
        {
            if (auto txt = GUI::getTypedElement<GUI::Text>(layer, label))
            {
                params.insert({label, txt});
            }
        };

        getParamTxt(CharacterParam::GuildName);
        getParamTxt(CharacterParam::LvlValue);
        getParamTxt(CharacterParam::ExpValue);
        getParamTxt(CharacterParam::NextLvlValue);
        getParamTxt(CharacterParam::SkillPointsValue);
        getParamTxt(CharacterParam::StrValue);
        getParamTxt(CharacterParam::DexValue);
        getParamTxt(CharacterParam::ManaValue);
        getParamTxt(CharacterParam::HpValue);
        getParamTxt(CharacterParam::RWeapon);
        getParamTxt(CharacterParam::RArrow);
        getParamTxt(CharacterParam::RFire);
        getParamTxt(CharacterParam::RMagic);
        getParamTxt(CharacterParam::OneHandValue);
        getParamTxt(CharacterParam::TwoHandValue);
        getParamTxt(CharacterParam::BowValue);
        getParamTxt(CharacterParam::CrossbowValue);
        getParamTxt(CharacterParam::MagicValue);
        getParamTxt(CharacterParam::OpenLocks);
        getParamTxt(CharacterParam::PickPocketing);
        getParamTxt(CharacterParam::SneakingValue);
        getParamTxt(CharacterParam::AcrobaticsValue);

        updateGui();
        updateGuiStats();
    }
    else
    {
        LOG_WARN << "Character viewer init gui error";
    }
}
void CharacterStatsComponent::updateGui()
{
}
void CharacterStatsComponent::show()
{
    mainWindow->activate(true);

    componentContext_.inputManager_.SetReleativeMouseMode(false);
    componentContext_.inputManager_.ShowCursor(true);

    if (auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera())
    {
        mainCamera->Lock();
        updateGui();
    }
}
void CharacterStatsComponent::hide()
{
    mainWindow->activate(false);

    componentContext_.inputManager_.SetReleativeMouseMode(true);
    componentContext_.inputManager_.ShowCursor(false);

    if (auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera())
    {
        mainCamera->Unlock();
    }
}
void CharacterStatsComponent::updateGuiStats()
{
    if (auto iter = params.find(CharacterParam::GuildName); iter != params.end())
    {
        auto str = magic_enum::enum_name(characterStats.general.guild);
        iter->second->setText(std::string(str));
    }

    auto setValue = [&]<typename T>(const char* param, T& value)
    {
        if (auto iter = params.find(param); iter != params.end())
        {
            iter->second->setText(std::to_string(value));
        }
    };

    // General
    setValue(CharacterParam::LvlValue, characterStats.general.level);
    setValue(CharacterParam::ExpValue, characterStats.general.exp);
    setValue(CharacterParam::NextLvlValue, characterStats.general.nextlvl);
    setValue(CharacterParam::SkillPointsValue, characterStats.general.skillPoints);

    // Attributes
    setValue(CharacterParam::StrValue, characterStats.attributes.str);
    setValue(CharacterParam::DexValue, characterStats.attributes.dex);
    setValue(CharacterParam::ManaValue, characterStats.attributes.mana);
    setValue(CharacterParam::HpValue, characterStats.attributes.life);

    // Protection
    setValue(CharacterParam::RWeapon, characterStats.protection.weapon);
    setValue(CharacterParam::RArrow, characterStats.protection.arrow);
    setValue(CharacterParam::RFire, characterStats.protection.fire);
    setValue(CharacterParam::RMagic, characterStats.protection.magic);

    // Fighting Skills
    setValue(CharacterParam::OneHandValue, characterStats.fightingSkills.oneHand);
    setValue(CharacterParam::TwoHandValue, characterStats.fightingSkills.twoHand);
    setValue(CharacterParam::BowValue, characterStats.fightingSkills.bow);
    setValue(CharacterParam::CrossbowValue, characterStats.fightingSkills.crossbow);
    setValue(CharacterParam::MagicValue, characterStats.fightingSkills.magic);

    // Thieving Skills
    setValue(CharacterParam::OpenLocks, characterStats.thievingSkills.openLocks);
    setValue(CharacterParam::PickPocketing, characterStats.thievingSkills.pickpocketing);
    setValue(CharacterParam::SneakingValue, characterStats.thievingSkills.sneaking);

    // Special Skills
    setValue(CharacterParam::AcrobaticsValue, characterStats.specialSkills.acrobatics);
}
}  // namespace Components
}  // namespace GameEngine
