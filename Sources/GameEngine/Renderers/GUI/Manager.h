#pragma once
#include <Input/KeyCodes.h>

#include <functional>
#include <list>
#include <magic_enum/magic_enum.hpp>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Animation.h"
#include "Element.h"
#include "Input/KeysSubscriptionsManager.h"
#include "Layer/Layer.h"
#include "Sprite/Sprite.h"
#include "Text/Text.h"

namespace Input
{
class InputManager;
}

namespace GameEngine
{
namespace GUI
{
using Layers         = std::unordered_map<std::string, Layer, StringViewHash, std::equal_to<>>;
using GuiElementsMap = std::unordered_map<std::string, Element*>;

class ENGINE_API Manager
{
public:
    Manager(Input::InputManager&);
    ~Manager();

    Layer& createLayer(std::string_view);
    void add(std::unique_ptr<Element>);
    void add(Animation);
    void update(float);
    void removeLayer(std::string_view);
    void removeAllFromLayer(const std::string&);
    void removeLayersExpect(const std::set<std::string>& = {});
    void remove(IdType);
    bool remove(const Element&);
    void removeAll();
    void bringToFront(const std::string&);
    Layer* getLayer(std::string_view);
    const Layers& getLayers() const;
    Layers& getLayers();
    void addTask(std::function<void()>);

private:
    void inputUpdate();
    Element* getElement(const Label&);
    Element* getElement(IdType);

private:
    Input::InputManager& inputmanager;
    Layers layers_;
    std::vector<std::string> layerOrder_;
    Layer* defaultLayer;
    std::mutex taskMutex_;
    std::mutex elementMutex_;

    std::vector<std::function<void()>> tasks_;
    std::vector<Animation> animations;

    Input::KeysSubscriptionsManager keySubManager;
    vec2 scrollDelta{0.f, 0.f};

private:
    std::optional<IdType> hoveredElementId;
    std::optional<IdType> pressedElementId;
    bool lastFrameLeftMouseButtonDown = false;
};
}  // namespace GUI
}  // namespace GameEngine
