#pragma once
#include <wx/wx.h>

#include <filesystem>
#include <string>


namespace GameEngine
{
class GameObject;
class File;
}

wxImage createImage(const unsigned char* data, unsigned int len, const wxSize& size);
bool is3dModelFile(const std::string&);
bool is3dModelFile(const std::filesystem::path&);
bool is3dModelFile(const GameEngine::File&);
bool isPrefab(const GameEngine::File&);
wxString GetParentPath(const wxString&);
void runAnimationViewer(const std::string& = std::string());
bool isGameObjectPrefab(const GameEngine::GameObject&);