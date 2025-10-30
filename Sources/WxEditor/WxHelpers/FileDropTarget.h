#pragma once
#include <wx/dnd.h>
#include <functional>

class FileDropTarget : public wxFileDropTarget
{
public:
    using Callback = std::function<void(const std::string&)>;
    FileDropTarget(Callback);

    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString&) override;

private:
    Callback callback;
};