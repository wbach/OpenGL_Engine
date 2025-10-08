#pragma once
#include <wx/wx.h>

#include <filesystem>
#include <string>

namespace GameEngine
{
class GameObject;
class File;
}  // namespace GameEngine

wxImage createImage(const unsigned char* data, unsigned int len, const wxSize& size);
bool is3dModelFile(const std::string&);
bool is3dModelFile(const std::filesystem::path&);
bool is3dModelFile(const GameEngine::File&);
bool isPrefab(const GameEngine::File&);
bool isMaterial(const GameEngine::File&);
wxString GetParentPath(const wxString&);
void runAnimationViewer(const std::string& = std::string());
void runMaterialEditor(const std::string& = std::string());
bool isGameObjectPrefab(const GameEngine::GameObject&);
std::unique_ptr<wxTextEntryDialog> createEntryDialogWithSelectedText(wxWindow* parent, const wxString& message,
                                                                     const wxString& caption = wxGetTextFromUserPromptStr,
                                                                     const wxString& value   = wxEmptyString,
                                                                     long style              = wxTextEntryDialogStyle,
                                                                     const wxPoint& pos      = wxDefaultPosition);