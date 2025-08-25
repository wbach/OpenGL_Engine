#pragma once
#include <wx/wx.h>

namespace GameEngine
{
class GameObject;
}

class ReloadComponentLibEvent : public wxCommandEvent
{
public:
    ReloadComponentLibEvent(wxEventType eventType = wxEVT_NULL, int winid = 0)
        : wxCommandEvent(eventType, winid)
    {
    }

    wxEvent* Clone() const override
    {
        return new ReloadComponentLibEvent(*this);
    }
    void SetFile(const std::string& f)
    {
        file = f;
    }
    const std::string& GetFile() const
    {
        return file;
    }

    void SetGameObject(GameEngine::GameObject& go)
    {
        gameObject = &go;
    }

    GameEngine::GameObject& GetGameObject()
    {
        return *gameObject;
    }

private:
    std::string file;
    GameEngine::GameObject* gameObject{nullptr};
};

wxDECLARE_EVENT(wxEVT_RELOAD_COMPONENT_LIB_EVENT, ReloadComponentLibEvent);
