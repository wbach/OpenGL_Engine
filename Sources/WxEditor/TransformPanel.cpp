#include "TransformPanel.h"

#include <Logger/Log.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
//#include <format>
#include <GameEngine/Objects/GameObject.h>
#include <wx/notebook.h>

#include <iomanip>  // std::setprecision
#include <sstream>

const std::string LABEL_POSITION{"Position"};
const std::string LABEL_ROTATION{"Rotation"};
const std::string LABEL_SCALE{"Scale"};

namespace
{
void setVectorControls(TransformPanel::Vector3Controls& controls, const vec3& vector)
{
    //    controls.xCtrl->SetValue(wxString::FromUTF8(std::format("{:.3f}", vector.x)));
    //    controls.yCtrl->SetValue(wxString::FromUTF8(std::format("{:.3f}", vector.y)));
    //    controls.zCtrl->SetValue(wxString::FromUTF8(std::format("{:.3f}", vector.z)));

    std::stringstream ssX, ssY, ssZ;

    ssX << std::fixed << std::setprecision(3) << vector.x;
    ssY << std::fixed << std::setprecision(3) << vector.y;
    ssZ << std::fixed << std::setprecision(3) << vector.z;

    controls.isSetBySub = true;
    controls.xCtrl->SetValue(ssX.str());
    controls.yCtrl->SetValue(ssY.str());
    controls.zCtrl->SetValue(ssZ.str());
    controls.isSetBySub = false;
}
}  // namespace

TransformPanel::TransformPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    position = CreateVector3Controls(this, LABEL_POSITION);
    rotation = CreateVector3Controls(this, LABEL_ROTATION);
    scale    = CreateVector3Controls(this, LABEL_SCALE);

    position.Bind();
    rotation.Bind();
    scale.Bind();

    wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);
    contentSizer->Add(position.sizer, 0, wxEXPAND | wxALL, 5);
    contentSizer->Add(rotation.sizer, 0, wxEXPAND | wxALL, 5);
    contentSizer->Add(scale.sizer, 0, wxEXPAND | wxALL, 5);

    SetSizer(contentSizer);
    Layout();
}

TransformPanel::Vector3Controls TransformPanel::CreateVector3Controls(wxWindow* parent, const wxString& label)
{
    wxStaticBox* box           = new wxStaticBox(parent, wxID_ANY, label);
    wxStaticBoxSizer* boxSizer = new wxStaticBoxSizer(box, wxVERTICAL);

    wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);

    rowSizer->Add(new wxStaticText(parent, wxID_ANY, "X:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    auto xCtrl = new wxTextCtrl(parent, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    rowSizer->Add(xCtrl, 1, wxRIGHT, 10);

    rowSizer->Add(new wxStaticText(parent, wxID_ANY, "Y:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    auto yCtrl = new wxTextCtrl(parent, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    rowSizer->Add(yCtrl, 1, wxRIGHT, 10);

    rowSizer->Add(new wxStaticText(parent, wxID_ANY, "Z:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    auto zCtrl = new wxTextCtrl(parent, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    rowSizer->Add(zCtrl, 1);

    boxSizer->Add(rowSizer, 0, wxEXPAND | wxALL, 5);

    Vector3Controls controls{
        .transformPanel = this, .xCtrl = xCtrl, .yCtrl = yCtrl, .zCtrl = zCtrl, .sizer = boxSizer, .label = label.ToStdString()};

    return controls;
}

void TransformPanel::set(const common::Transform& transform)
{
    setVectorControls(position, transform.GetPosition());
    setVectorControls(rotation, transform.GetRotation().GetEulerDegrees().value);
    setVectorControls(scale, transform.GetScale());
}

void TransformPanel::set(TransfromChanged callback)
{
    transfromChanged = callback;
}

void TransformPanel::lock()
{
    std::function<void(wxWindow*)> lockChildren = [&](wxWindow* parent)
    {
        if (not parent)
            return;

        parent->Disable();

        for (auto child : parent->GetChildren())
            lockChildren(child);
    };

    for (auto child : GetChildren())
        lockChildren(child);
}

void TransformPanel::unlock()
{
    std::function<void(wxWindow*)> enableChildren;
    enableChildren = [&](wxWindow* parent)
    {
        if (not parent)
            return;

        parent->Enable();
        wxWindowList& children = parent->GetChildren();
        for (auto child : children)
        {
            enableChildren(child);
        }
    };
    enableChildren(this);
}

void TransformPanel::Vector3Controls::onChanged(wxCommandEvent& event)
{
    if (isSetBySub or not transformPanel->transfromChanged)
        return;

    try
    {
        auto x = std::stof(xCtrl->GetValue().ToStdString());
        auto y = std::stof(yCtrl->GetValue().ToStdString());
        auto z = std::stof(zCtrl->GetValue().ToStdString());
        vec3 v(x, y, z);
        transformPanel->transfromChanged(label, v);
        tmpFocusString.clear();
    }
    catch (...)
    {
        ERROR_LOG("Parse error");
    }
}

void TransformPanel::Vector3Controls::onFocus(wxFocusEvent& event)
{
    if (event.GetEventObject() == xCtrl)
    {
        tmpFocusString = xCtrl->GetValue();
    }
    else if (event.GetEventObject() == yCtrl)
    {
        tmpFocusString = yCtrl->GetValue();
    }
    else if (event.GetEventObject() == zCtrl)
    {
        tmpFocusString = zCtrl->GetValue();
    }
}

void TransformPanel::Vector3Controls::onKillFocus(wxFocusEvent& event)
{
    if (tmpFocusString.empty())
        return;

    if (event.GetEventObject() == xCtrl)
    {
        xCtrl->SetValue(tmpFocusString);
    }
    else if (event.GetEventObject() == yCtrl)
    {
        yCtrl->SetValue(tmpFocusString);
    }
    else if (event.GetEventObject() == zCtrl)
    {
        zCtrl->SetValue(tmpFocusString);
    }
    tmpFocusString.clear();
}

void TransformPanel::Vector3Controls::Bind()
{
    xCtrl->Bind(wxEVT_TEXT_ENTER, &Vector3Controls::onChanged, this);
    yCtrl->Bind(wxEVT_TEXT_ENTER, &Vector3Controls::onChanged, this);
    zCtrl->Bind(wxEVT_TEXT_ENTER, &Vector3Controls::onChanged, this);

    xCtrl->Bind(wxEVT_SET_FOCUS, &Vector3Controls::onFocus, this);
    yCtrl->Bind(wxEVT_SET_FOCUS, &Vector3Controls::onFocus, this);
    zCtrl->Bind(wxEVT_SET_FOCUS, &Vector3Controls::onFocus, this);

    xCtrl->Bind(wxEVT_KILL_FOCUS, &Vector3Controls::onKillFocus, this);
    yCtrl->Bind(wxEVT_KILL_FOCUS, &Vector3Controls::onKillFocus, this);
    zCtrl->Bind(wxEVT_KILL_FOCUS, &Vector3Controls::onKillFocus, this);
}
