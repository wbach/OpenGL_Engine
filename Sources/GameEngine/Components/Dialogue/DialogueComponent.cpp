#include "DialogueComponent.h"

#include <Utils/TreeNode.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Scene/Scene.hpp"
namespace GameEngine
{
namespace
{
constexpr char CSTR_DIALOG_FILE[]   = "dialogueFile";
constexpr char CSTR_START_NODE_ID[] = "startNodeID";
}  // namespace

namespace Components
{

DialogueComponent::DialogueComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<DialogueComponent>(), componentContext, gameObject)
{
}
void DialogueComponent::CleanUp()
{
}
void DialogueComponent::Reload()
{
}
void DialogueComponent::ReqisterFunctions()
{
}
void DialogueComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<DialogueComponent>(componentContext, gameObject);
        component->read(node);
        std::filesystem::path filePath;
        ::Read(node.getChild(CSTR_DIALOG_FILE), filePath);
        component->dialogueFile = filePath;

        ::Read(node.getChild(CSTR_START_NODE_ID), component->startNodeID);
        // if (component->dialogueFile.exist())
        {
            component->readFile();
        }
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<DialogueComponent>(), func);
}
void DialogueComponent::write(TreeNode& node) const
{
    const std::string CSTR_TYPE = "type";
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(CSTR_DIALOG_FILE), dialogueFile.GetDataRelativePath());
    ::write(node.addChild(CSTR_START_NODE_ID), startNodeID);
}
void DialogueComponent::readFile()
{
    // demo
    nodes[0] = {0, "Stoj! Kto idzie?", {{"Szukam schronienia.", 2, ""}, {"Nie Twoj interes.", -1, ""}}};
    nodes[1] = {1, "O co chodzi?.", {{"Co to za oboz?", 2, ""}, {"Powiedz mi wiecej o obozie.", 3, ""}}};
    nodes.at(currentNodeID);
    nodes[2] = {2,
                "To Oboz Cienia. Swiat oszalal. Chcesz wiedziec wiecej?",
                {{"Tak, opowiedz mi.", 3, ""}, {"Nie, musze leciec.", -1, ""}}};
    nodes[3] = {3, "Duzo by gadac..., ale wydajesz sie porzadny pogadamy w srodku.", {{"Dziekuje!", -2, "invited_to_camp"}}};
    nodes[4] = {4, "Do zobaczenia w srodku.", {{"Do zobaczenia", -2, "invited_to_camp"}}};
}
DialogueComponent::SelectOptionResult DialogueComponent::selectOption(int optionIndex)
{
    LOG_DEBUG << "selectOption " << optionIndex;

    DialogueOption& selected = nodes[currentNodeID].options[optionIndex];
    if (not selected.actionFlag.empty())
    {
        componentContext_.scene_.getEngineContext()->GetGameState().flags[selected.actionFlag] = 1;
    }

    if (selected.nextNodeID == -1)
    {
        LOG_DEBUG << "End dialog";
        currentNodeID = 1;
        return SelectOptionResult::end;
    }
    else if (selected.nextNodeID == -2)
    {
        LOG_DEBUG << "End dialog";
        currentNodeID = 4;
        return SelectOptionResult::end;
    }
    else
    {
        currentNodeID = selected.nextNodeID;
    }

    return SelectOptionResult::active;
}

const DialogueNode* DialogueComponent::getCurrent() const
{
    auto iter = nodes.find(currentNodeID);
    if (iter != nodes.end())
    {
        return &iter->second;
    }

    LOG_DEBUG << "Current not set! currentNodeID = " << currentNodeID;
    return nullptr;
}
}  // namespace Components
}  // namespace GameEngine
