#pragma once
#include <wx/wx.h>

namespace GameEngine
{
class GameObject;
}

class ReloadComponentLibEvent : public wxCommandEvent
{
public:
    ReloadComponentLibEvent(wxEventType eventType = wxEVT_NULL, int winid = 0);

    wxEvent* Clone() const override;
    void SetFile(const std::string& f);
    const std::string& GetFile() const;
    void SetGameObject(GameEngine::GameObject& go);
    GameEngine::GameObject& GetGameObject();

private:
    std::string file;
    GameEngine::GameObject* gameObject{nullptr};
};

wxDECLARE_EVENT(wxEVT_RELOAD_COMPONENT_LIB_EVENT, ReloadComponentLibEvent);
