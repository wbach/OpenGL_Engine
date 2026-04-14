#pragma once

#include "GameEngine/Renderers/GUI/Element.h"
namespace GameEngine
{
namespace GUI
{
class Element;
class Text;
class MultiLineText;
class Sprite;
class Button;
class Window;
class EditText;
class VerticalLayout;
class HorizontalLayout;

class IElementVisitor
{
public:
    virtual ~IElementVisitor() = default;

    virtual void visit(Element&)          = 0;
    virtual void visit(Text&)             = 0;
    virtual void visit(MultiLineText&)    = 0;
    virtual void visit(Sprite&)           = 0;
    virtual void visit(Button&)           = 0;
    virtual void visit(Window&)           = 0;
    virtual void visit(EditText&)         = 0;
    virtual void visit(VerticalLayout&)   = 0;
    virtual void visit(HorizontalLayout&) = 0;
};
}  // namespace GUI
}  // namespace GameEngine
