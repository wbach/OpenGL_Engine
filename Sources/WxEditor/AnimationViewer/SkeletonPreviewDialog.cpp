#include "SkeletonPreviewDialog.h"

SkeletonPreviewDialog::SkeletonPreviewDialog(wxWindow* parent, const GameEngine::Animation::Skeleton& skeleton,
                                             const std::string& title, Mode mode)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 900))
    , mode_(mode)
    , skeletonCopy_(skeleton.getRootJoint())
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    UpdateHierarchy(skeletonCopy_, glm::mat4(1.0f));

    viewOffset_ = wxPoint(500, 800);

    Bind(wxEVT_PAINT, &SkeletonPreviewDialog::OnPaint, this);
    Bind(wxEVT_MOUSEWHEEL, &SkeletonPreviewDialog::OnMouseWheel, this);
    Bind(wxEVT_MOTION, &SkeletonPreviewDialog::OnMouseMove, this);
    Bind(wxEVT_RIGHT_DOWN, &SkeletonPreviewDialog::OnRightDown, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});

    if (mode_ == Mode::SINGLE_SELECTION)
    {
        Bind(wxEVT_LEFT_DCLICK, &SkeletonPreviewDialog::TrySelectSingle, this);
    }
}
const std::string& SkeletonPreviewDialog::GetSelectedName() const
{
    return selectedName_;
}
GameEngine::Animation::JointId SkeletonPreviewDialog::GetSelectedId() const
{
    return selectedId_;
}
bool SkeletonPreviewDialog::WasJointSelected() const
{
    return jointSelected_;
}
void SkeletonPreviewDialog::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    DrawJoint(dc, skeletonCopy_);

    dc.SetTextForeground(*wxLIGHT_GREY);
    dc.DrawText(BuildHelpText(), 10, 10);
}
void SkeletonPreviewDialog::DrawJoint(wxDC& dc, const GameEngine::Animation::Joint& joint)
{
    wxPoint currentPos = WorldToScreen(joint.worldTransform);

    for (const GameEngine::Animation::Joint& child : joint.children)
    {
        wxPoint childPos = WorldToScreen(child.worldTransform);

        dc.SetPen(wxPen(wxColour(100, 150, 255), 2));
        dc.DrawLine(currentPos, childPos);

        DrawJoint(dc, child);
    }

    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(joint.children.empty() ? *wxRED_BRUSH : *wxCYAN_BRUSH);
    dc.DrawCircle(currentPos, 4);

    if (scale_ > 60.0f)
    {
        dc.SetTextForeground(wxColour(220, 220, 220));
        dc.DrawText(joint.name, currentPos.x + 8, currentPos.y - 12);
    }
}
wxPoint SkeletonPreviewDialog::WorldToScreen(const mat4& worldMatrix) const
{
    return wxPoint(static_cast<int>(worldMatrix[3][0] * scale_) + viewOffset_.x,
                   static_cast<int>(-worldMatrix[3][1] * scale_) + viewOffset_.y);
}
void SkeletonPreviewDialog::OnMouseWheel(wxMouseEvent& event)
{
    const float zoomStep = 1.15f;
    const float oldScale = scale_;

    if (event.GetWheelRotation() > 0)
        scale_ *= zoomStep;
    else
        scale_ /= zoomStep;

    if (scale_ < 1.0f)
        scale_ = 1.0f;
    if (scale_ > 20000.0f)
        scale_ = 20000.0f;

    wxPoint mousePos = event.GetPosition();
    float factor     = scale_ / oldScale;
    viewOffset_.x    = mousePos.x - static_cast<int>((mousePos.x - viewOffset_.x) * factor);
    viewOffset_.y    = mousePos.y - static_cast<int>((mousePos.y - viewOffset_.y) * factor);

    Refresh();
}
void SkeletonPreviewDialog::OnRightDown(wxMouseEvent& event)
{
    lastMousePos_ = event.GetPosition();
}
void SkeletonPreviewDialog::OnMouseMove(wxMouseEvent& event)
{
    if (event.RightIsDown())
    {
        wxPoint currentPos = event.GetPosition();
        viewOffset_ += (currentPos - lastMousePos_);
        lastMousePos_ = currentPos;
        Refresh();
    }
}
void SkeletonPreviewDialog::UpdateHierarchy(GameEngine::Animation::Joint& joint, const mat4& parentTransform)
{
    joint.worldTransform = (joint.ignoreParentRotation or false) ? joint.transform : parentTransform * joint.transform;

    for (GameEngine::Animation::Joint& child : joint.children)
    {
        UpdateHierarchy(child, joint.worldTransform);
    }
}
const GameEngine::Animation::Joint* SkeletonPreviewDialog::FindJointAt(const GameEngine::Animation::Joint& joint,
                                                                       wxPoint mousePos)
{
    wxPoint jointPos = WorldToScreen(joint.worldTransform);

    float dx         = static_cast<float>(mousePos.x - jointPos.x);
    float dy         = static_cast<float>(mousePos.y - jointPos.y);
    float distanceSq = dx * dx + dy * dy;

    if (distanceSq < 100.0f)
    {
        return &joint;
    }

    for (const auto& child : joint.children)
    {
        const GameEngine::Animation::Joint* found = FindJointAt(child, mousePos);
        if (found != nullptr)
            return found;
    }

    return nullptr;
}
void SkeletonPreviewDialog::TrySelectSingle(wxMouseEvent& event)
{
    const GameEngine::Animation::Joint* clickedJoint = FindJointAt(skeletonCopy_, event.GetPosition());

    if (clickedJoint != nullptr)
    {
        selectedName_  = clickedJoint->name;
        selectedId_    = clickedJoint->id;
        jointSelected_ = true;

        EndModal(wxID_OK);
    }
}
std::string SkeletonPreviewDialog::BuildHelpText() const
{
    std::string help = "RMB: Pan | Scroll: Zoom";

    if (mode_ == Mode::SINGLE_SELECTION)
    {
        help += " | Double Click: Select Joint";
    }

    return help;
}
