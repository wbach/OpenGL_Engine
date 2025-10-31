#include "ReloadComponentLibEvent.h"

wxDEFINE_EVENT(wxEVT_RELOAD_COMPONENT_LIB_EVENT, ReloadComponentLibEvent);

ReloadComponentLibEvent::ReloadComponentLibEvent(wxEventType eventType, int winid)
    : wxCommandEvent(eventType, winid)
{
}
wxEvent* ReloadComponentLibEvent::Clone() const
{
    return new ReloadComponentLibEvent(*this);
}
void ReloadComponentLibEvent::SetFile(const std::filesystem::path& f)
{
    file = f;
}
const std::filesystem::path& ReloadComponentLibEvent::GetFile() const
{
    return file;
}
void ReloadComponentLibEvent::SetGameObject(GameEngine::GameObject& go)
{
    gameObject = &go;
}
GameEngine::GameObject& ReloadComponentLibEvent::GetGameObject()
{
    return *gameObject;
}
