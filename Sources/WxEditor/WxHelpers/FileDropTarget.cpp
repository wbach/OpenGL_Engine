#include "FileDropTarget.h"

FileDropTarget::FileDropTarget(Callback callback)
    : callback{callback}
{
}
bool FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    if (not filenames.IsEmpty())
    {
        if (callback)
            callback(filenames[0].ToStdString());
    }
    return true;
}
