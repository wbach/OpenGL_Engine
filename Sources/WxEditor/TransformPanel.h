#pragma once
#include <Common/Transform.h>
#include <wx/collpane.h>
#include <wx/panel.h>
#include <wx/wx.h>

extern const std::string LABEL_POSITION;
extern const std::string LABEL_ROTATION;
extern const std::string LABEL_SCALE;

class TransformPanel : public wxPanel
{
public:
    struct Vector3Controls
    {
        TransformPanel* transformPanel;
        wxTextCtrl* xCtrl{nullptr};
        wxTextCtrl* yCtrl{nullptr};
        wxTextCtrl* zCtrl{nullptr};
        wxSizer* sizer{nullptr};  // zeby mozna bylo latwo dodac do layoutu
        std::string label;

        wxString tmpFocusString;

        void onChanged(wxCommandEvent&);
        void onFocus(wxFocusEvent&);
        void onKillFocus(wxFocusEvent&);
        bool isSetBySub{false};
        void Bind();
    };
    using TransfromChanged = std::function<void(const std::string&, const vec3&)>;

    TransformPanel(wxWindow* parent);
    void set(const common::Transform&);
    void set(TransfromChanged);
    void lock();
    void unlock();

private:
    void OnPaneChanged(wxCollapsiblePaneEvent&);
    Vector3Controls CreateVector3Controls(wxWindow*, const wxString&);

private:
    common::Transform* tranfrom{nullptr};
    TransfromChanged transfromChanged;

    wxCollapsiblePane* posPane{nullptr};
    wxCollapsiblePane* rotPane{nullptr};
    wxCollapsiblePane* scalePane{nullptr};

    Vector3Controls position;
    Vector3Controls rotation;
    Vector3Controls scale;
};
