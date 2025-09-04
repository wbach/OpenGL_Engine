#pragma once
#include <Types.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wx.h>

#include "GLCanvas.h"

class AnimationViewerFrame : public wxFrame
{
public:
    AnimationViewerFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void Init();

private:
    GLCanvas* m_canvas;
    wxListBox* m_animList;
};
