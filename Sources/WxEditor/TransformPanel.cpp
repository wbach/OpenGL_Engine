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
    collapsible = new wxCollapsiblePane(this, wxID_ANY, "transform");

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(collapsible, 1, wxEXPAND | wxALL, 0);

    wxWindow* pane = collapsible->GetPane();

    position = CreateVector3Controls(pane, LABEL_POSITION);
    rotation = CreateVector3Controls(pane, LABEL_ROTATION);
    scale    = CreateVector3Controls(pane, LABEL_SCALE);

    position.Bind();
    rotation.Bind();
    scale.Bind();

    wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);
    contentSizer->Add(position.sizer, 0, wxEXPAND | wxALL, 5);
    contentSizer->Add(rotation.sizer, 0, wxEXPAND | wxALL, 5);
    contentSizer->Add(scale.sizer, 0, wxEXPAND | wxALL, 5);

    pane->SetSizer(contentSizer);

    SetSizer(mainSizer);
    Layout();

    collapsible->Collapse(false);
    collapsible->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &TransformPanel::OnCollapsibleChanged, this);
}

void TransformPanel::OnCollapsibleChanged(wxCollapsiblePaneEvent& event)
{
    bool collapsed = collapsible->IsCollapsed();

    wxNotebook* notebook = dynamic_cast<wxNotebook*>(GetParent());
    if (!notebook)
        return;

    int pageCount = notebook->GetPageCount();
    for (int i = 0; i < pageCount; i++)
    {
        wxWindow* page = notebook->GetPage(i);
        if (page == this)  // pomijamy bieżącą kartę
            continue;

        TransformPanel* tp = dynamic_cast<TransformPanel*>(page);
        if (tp)
        {
            if (tp->collapsible->IsCollapsed() != collapsed)
            {
                tp->collapsible->Collapse(collapsed);
                tp->Layout();
            }
        }
    }

    notebook->Layout();
    if (notebook->GetParent())
        notebook->GetParent()->Layout();
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
