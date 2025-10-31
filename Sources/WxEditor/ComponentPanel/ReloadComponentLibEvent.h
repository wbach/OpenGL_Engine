#pragma once
#include <wx/wx.h>
#include <filesystem>

namespace GameEngine
{
class GameObject;
}

class ReloadComponentLibEvent : public wxCommandEvent
{
public:
    ReloadComponentLibEvent(wxEventType eventType = wxEVT_NULL, int winid = 0);

    wxEvent* Clone() const override;
    void SetFile(const std::filesystem::path&);
    const std::filesystem::path& GetFile() const;
    void SetGameObject(GameEngine::GameObject&);
    GameEngine::GameObject& GetGameObject();

private:
    std::filesystem::path file;
    GameEngine::GameObject* gameObject{nullptr};
};

wxDECLARE_EVENT(wxEVT_RELOAD_COMPONENT_LIB_EVENT, ReloadComponentLibEvent);
