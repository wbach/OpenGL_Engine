#include "Manager.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Types.h>

#include <algorithm>
#include <string>
#include <utility>

#include "ElementWriter.h"
#include "ElementsDef.h"
#include "GameEngine/Renderers/GUI/Element.h"
#include "Input/KeyCodes.h"

namespace GameEngine
{
namespace GUI
{
namespace
{
bool isCollision(const Element& element, const vec2& mousePosition)
{
    if (not element.isActive())
        return false;

    auto convertedMousePosition = (mousePosition + 1.f) / 2.f;
    auto screenPosition         = element.getScreenPosition();
    auto halfScreenScale        = element.getScreenScale() / 2.f;

    return convertedMousePosition.x >= screenPosition.x - halfScreenScale.x and
           convertedMousePosition.x <= screenPosition.x + halfScreenScale.x and
           convertedMousePosition.y >= screenPosition.y - halfScreenScale.y and
           convertedMousePosition.y <= screenPosition.y + halfScreenScale.y;
}

Element* getCollisonElement(Element& element, const vec2& mousePosition)
{
    auto tmin = std::numeric_limits<float>::max();
    Element* result{nullptr};

    for (auto& child : element.getChildren())
    {
        if (not child->isActive())
        {
            continue;
        }

        if (auto toReturn = getCollisonElement(*child, mousePosition))
        {
            if (toReturn->getZValue() < tmin)
            {
                tmin   = toReturn->getZValue();
                result = toReturn;
            }
        }
    }

    return result ? result : (isCollision(element, mousePosition) ? &element : nullptr);
}
}  // namespace
Manager::Manager(Input::InputManager& manger)
    : inputmanager(manger)
    , keySubManager(manger)
{
    defaultLayer = &createLayer(DEFAULT_LAYER);

    keySubManager = inputmanager.SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [this]() { scrollDelta.y = -1.f; });
    keySubManager = inputmanager.SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [this]() { scrollDelta.y = 1.f; });
}

Manager::~Manager()
{
    LOG_DEBUG << "destructor";
    keySubManager.UnsubscribeKeys();
}
Layer& Manager::createLayer(std::string_view name)
{
    auto [it, inserted] = layers_.try_emplace(std::string{name}, name);

    if (not inserted)
    {
        LOG_WARN << "Layer with name: " << name << " already exists";
    }
    else
    {
        layerOrder_.push_back(it->first);
    }

    return it->second;
}

void Manager::add(std::unique_ptr<Element> element)
{
    defaultLayer->add(std::move(element));
}
void Manager::add(Animation element)
{
    animations.push_back(std::move(element));
}
Element* Manager::getElement(const Label& label)
{
    for (auto& [_, layer] : layers_)
    {
        if (auto el = layer.get(label))
        {
            return el;
        }
    }

    LOG_WARN << "Element with label " << label << " not found";
    return nullptr;
}
Element* Manager::getElement(IdType id)
{
    for (auto& [_, layer] : layers_)
    {
        if (auto el = layer.get(id))
        {
            return el;
        }
    }
    return nullptr;
}
const Layers& Manager::getLayers() const
{
    return layers_;
}
Layers& Manager::getLayers()
{
    return layers_;
}
void Manager::update(float deltaTime)
{
    for (auto iter = animations.begin(); iter != animations.end();)
    {
        if (iter->update(deltaTime) == Animation::Status::done)
        {
            iter = animations.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    {
        std::lock_guard<std::mutex> lk(elementMutex_);
        for (auto& [_, layer] : layers_)
        {
            layer.refresh();

            for (auto& element : layer.get())
            {
                element->update();
            }
        }
    }

    std::vector<std::function<void()>> tmpTasks;
    {
        std::lock_guard<std::mutex> lk(taskMutex_);
        tmpTasks = std::move(tasks_);
    }

    for (auto& task : tmpTasks)
    {
        task();
    }

    inputUpdate();
}

void Manager::addTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> lk(taskMutex_);
    tasks_.push_back(task);
}

void Manager::remove(IdType id)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    auto element = getElement(id);
    if (element)
    {
        for (auto& [_, layer] : layers_)
        {
            if (layer.removeElement(*element))
                return;
        }
    }
    else
    {
        LOG_WARN << "Element not found";
    }
}

bool Manager::remove(const Element& element)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    for (auto& [_, layer] : layers_)
    {
        if (layer.removeElement(element))
            return true;
    }

    return false;
}

void Manager::removeAll()
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    for (auto& [_, layer] : layers_)
    {
        layer.clear();
    }
}

Layer* Manager::getLayer(std::string_view name)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    auto iter = layers_.find(name);
    if (iter != layers_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

void Manager::removeLayer(std::string_view layerName)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    auto iter = layers_.find(layerName);
    if (iter != layers_.end())
    {
        layers_.erase(iter);
    }
    layerOrder_.erase(std::remove(layerOrder_.begin(), layerOrder_.end(), layerName), layerOrder_.end());
}

void Manager::removeLayersExpect(const std::set<std::string>& exceptions)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    if (exceptions.empty())
    {
        layers_.clear();
        layerOrder_.clear();
    }
    else
    {
        for (auto iter = layers_.begin(); iter != layers_.end();)
        {
            const auto& layerName = iter->first;

            auto skip = exceptions.find(layerName);
            if (skip == exceptions.end())
            {
                iter = layers_.erase(iter);
                layerOrder_.erase(std::remove(layerOrder_.begin(), layerOrder_.end(), layerName), layerOrder_.end());
            }
            else
            {
                ++iter;
            }
        }
    }
}

void Manager::removeAllFromLayer(const std::string& layerName)
{
    if (auto layer = getLayer(layerName))
    {
        layer->clear();
    }
}
void Manager::inputUpdate()
{
    auto mousePosition = inputmanager.GetMousePosition();
    // std::lock_guard<std::mutex> lk(elementMutex_);

    Element* currentUnderMouse = nullptr;

    for (auto it = layerOrder_.rbegin(); it != layerOrder_.rend(); ++it)
    {
        const std::string& layerName = *it;
        auto layer                   = getLayer(layerName);
        if (not layer)
        {
            LOG_WARN << "Layer not found, somethi goes wrong";
            continue;
        }

        if (not layer->isActive())
            continue;

        for (auto& element : layer->get())
        {
            auto found = getCollisonElement(*element, mousePosition);
            if (found)
            {
                currentUnderMouse = found;
                break;
            }
        }
        if (currentUnderMouse)
            break;
    }

    std::optional<IdType> currentId;
    if (currentUnderMouse)
    {
        currentId = currentUnderMouse->getId();
    }

    if (currentId != hoveredElementId)
    {
        if (hoveredElementId.has_value())
        {
            if (auto oldElement = getElement(*hoveredElementId))
            {
                oldElement->onMouseLeave();
            }
        }

        if (currentUnderMouse)
        {
            currentUnderMouse->onMouseEnter();
        }

        hoveredElementId = currentId;
    }
    else if (hoveredElementId.has_value())
    {
        if (auto el = getElement(*hoveredElementId))
        {
            el->onMousePositionUpdate(mousePosition);
        }
        else
        {
            hoveredElementId.reset();
        }
    }

    bool isCurrentlyDown = inputmanager.GetKey(KeyCodes::LMOUSE);

    if (isCurrentlyDown && !lastFrameLeftMouseButtonDown)
    {
        if (currentId.has_value())
        {
            pressedElementId = currentId;
            // getElement(*pressedElementId)->onMouseDown();
        }
    }
    else if (!isCurrentlyDown && lastFrameLeftMouseButtonDown)
    {
        if (pressedElementId.has_value())
        {
            if (currentId == pressedElementId)
            {
                if (currentUnderMouse)
                {
                    currentUnderMouse->onMouseClick(mousePosition, KeyCodes::LMOUSE);
                }
            }

            if (auto el = getElement(*pressedElementId))
            {
                el->onMouseClickRelease(KeyCodes::LMOUSE);
            }

            pressedElementId.reset();
        }
    }

    if (currentUnderMouse and std::abs(scrollDelta.y) > 0.1f)
    {
        currentUnderMouse->onMouseWheel(scrollDelta);
    }

    scrollDelta = {0.f, 0.f};

    lastFrameLeftMouseButtonDown = isCurrentlyDown;
}
void Manager::bringToFront(const std::string& name)
{
    auto it = std::find(layerOrder_.begin(), layerOrder_.end(), name);
    if (it != layerOrder_.end())
    {
        std::rotate(it, it + 1, layerOrder_.end());
    }
}
}  // namespace GUI
}  // namespace GameEngine
