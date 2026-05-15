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

    virtual void visit(const Element&)          = 0;
    virtual void visit(const Text&)             = 0;
    virtual void visit(const MultiLineText&)    = 0;
    virtual void visit(const Sprite&)           = 0;
    virtual void visit(const Button&)           = 0;
    virtual void visit(const Window&)           = 0;
    virtual void visit(const EditText&)         = 0;
    virtual void visit(const VerticalLayout&)   = 0;
    virtual void visit(const HorizontalLayout&) = 0;
};
}  // namespace GUI
}  // namespace GameEngine
