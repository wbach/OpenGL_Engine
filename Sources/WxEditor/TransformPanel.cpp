#include "TransformPanel.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>

#include <Logger/Log.h>
//#include <format>
#include <sstream>
#include <iomanip>  // std::setprecision

namespace
{
void setVectorControls(TransformPanel::Vector3Controls& controls, const vec3& vector)
{
//    controls.xCtrl->SetValue(wxString::FromUTF8(std::format("{:.3f}", vector.x)));
//    controls.yCtrl->SetValue(wxString::FromUTF8(std::format("{:.3f}", vector.y)));
//    controls.zCtrl->SetValue(wxString::FromUTF8(std::format("{:.3f}", vector.z)));

    DEBUG_LOG(std::to_string(vector));
    std::stringstream ssX, ssY, ssZ;

    ssX << std::fixed << std::setprecision(3) << vector.x;
    ssY << std::fixed << std::setprecision(3) << vector.y;
    ssZ << std::fixed << std::setprecision(3) << vector.z;

    controls.xCtrl->SetValue(ssX.str());
    controls.yCtrl->SetValue(ssY.str());
    controls.zCtrl->SetValue(ssZ.str());
}
}  // namespace

TransformPanel::TransformPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    wxCollapsiblePane* collapsible = new wxCollapsiblePane(this, wxID_ANY, "transform");

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(collapsible, 1, wxEXPAND | wxALL, 5);

    wxWindow* pane = collapsible->GetPane();

    position = CreateVector3Controls(pane, "Position");
    rotation = CreateVector3Controls(pane, "Rotation");
    scale    = CreateVector3Controls(pane, "Scale");

    wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);
    contentSizer->Add(position.sizer, 0, wxEXPAND | wxALL, 5);
    contentSizer->Add(rotation.sizer, 0, wxEXPAND | wxALL, 5);
    contentSizer->Add(scale.sizer, 0, wxEXPAND | wxALL, 5);

    pane->SetSizer(contentSizer);

    SetSizer(mainSizer);
    Layout();

    collapsible->Collapse(false);
}

TransformPanel::Vector3Controls TransformPanel::CreateVector3Controls(wxWindow* parent, const wxString& label)
{
    Vector3Controls controls{};

    wxStaticBox* box           = new wxStaticBox(parent, wxID_ANY, label);
    wxStaticBoxSizer* boxSizer = new wxStaticBoxSizer(box, wxVERTICAL);

    wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);

    rowSizer->Add(new wxStaticText(parent, wxID_ANY, "X:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    controls.xCtrl = new wxTextCtrl(parent, wxID_ANY, "0");
    rowSizer->Add(controls.xCtrl, 1, wxRIGHT, 10);

    rowSizer->Add(new wxStaticText(parent, wxID_ANY, "Y:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    controls.yCtrl = new wxTextCtrl(parent, wxID_ANY, "0");
    rowSizer->Add(controls.yCtrl, 1, wxRIGHT, 10);

    rowSizer->Add(new wxStaticText(parent, wxID_ANY, "Z:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    controls.zCtrl = new wxTextCtrl(parent, wxID_ANY, "0");
    rowSizer->Add(controls.zCtrl, 1);

    boxSizer->Add(rowSizer, 0, wxEXPAND | wxALL, 5);

    controls.sizer = boxSizer;
    return controls;
}

void TransformPanel::set(const common::Transform& transform)
{
    setVectorControls(position, transform.GetPosition());
    setVectorControls(rotation, transform.GetRotation().GetEulerDegrees().value);
    setVectorControls(scale, transform.GetScale());
}
