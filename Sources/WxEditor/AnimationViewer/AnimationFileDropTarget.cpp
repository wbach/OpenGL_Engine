#include "AnimationFileDropTarget.h"

#include <Logger/Log.h>
#include "AnimationViewerFrame.h"

AnimationFileDropTarget::AnimationFileDropTarget(AnimationViewerFrame* panel)
    : viewer(panel)
{
}
bool AnimationFileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    LOG_DEBUG << "OnDropFiles";

    if (!viewer)
        return false;

    if (filenames.empty())
        return false;

    if (filenames.size() > 1)
    {
        LOG_DEBUG << "Only one file is supported";
    }

    const auto& file = filenames[0].ToStdString();

    LOG_DEBUG << "OnDropFiles: " << file;

    if (std::filesystem::is_directory(file))
    {
        if (not viewer->isObjectSelcted())
        {
            wxMessageBox("Dropp folder, but model not set", "Warning", wxOK | wxICON_WARNING);
            return false;
        }
        int answer = wxMessageBox("Dropp folder, search for animations there?", "Confirmation", wxYES_NO | wxICON_QUESTION);

        if (answer == wxYES)
        {
            viewer->SearchAndAddClipsFromDir(file);
        }
    }
    else
    {
        viewer->ShowModel(file);
    }

    return true;
}
