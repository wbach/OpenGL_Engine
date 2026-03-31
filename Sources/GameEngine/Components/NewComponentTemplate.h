#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
class GameState;
namespace Components
{
class ENGINE_API NewComponentTemplate : public BaseComponent
{
public:
public:
    // clang-format off
    BEGIN_FIELDS()
    END_FIELDS()
    // clang-format on

public:
    NewComponentTemplate(ComponentContext&, GameObject&);
    ~NewComponentTemplate() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;
    void readFile();

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
