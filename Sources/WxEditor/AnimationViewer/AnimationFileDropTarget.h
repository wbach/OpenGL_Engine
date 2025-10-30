#pragma once
#include <wx/dnd.h>

class AnimationViewerFrame;

class AnimationFileDropTarget : public wxFileDropTarget
{
public:
    AnimationFileDropTarget(AnimationViewerFrame*);
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString&) override;

private:
    AnimationViewerFrame* viewer;
};