#pragma once
#include <GameEngine/Animations/Skeleton.h>
#include <wx/dcbuffer.h>
#include <wx/wx.h>

#include <glm/gtc/matrix_access.hpp>

class SkeletonPreviewDialog : public wxDialog
{
public:
    SkeletonPreviewDialog(wxWindow* parent, const GameEngine::Animation::Skeleton& skeleton)
        : wxDialog(parent, wxID_ANY, "Skeleton Hierarchy Preview", wxDefaultPosition, wxSize(1000, 900))
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
    }

private:
    GameEngine::Animation::Joint skeletonCopy_;
    float scale_ = 300.0f;
    wxPoint viewOffset_;
    wxPoint lastMousePos_;

    void OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.SetBackground(*wxBLACK_BRUSH);
        dc.Clear();

        DrawJoint(dc, skeletonCopy_);

        dc.SetTextForeground(*wxLIGHT_GREY);
        dc.DrawText("Scroll: Zoom to Cursor | RMB: Pan", 10, 10);
    }

    void DrawJoint(wxDC& dc, const GameEngine::Animation::Joint& joint)
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

    wxPoint WorldToScreen(const mat4& worldMatrix) const
    {
        return wxPoint(static_cast<int>(worldMatrix[3][0] * scale_) + viewOffset_.x,
                       static_cast<int>(-worldMatrix[3][1] * scale_) + viewOffset_.y);
    }

    void OnMouseWheel(wxMouseEvent& event)
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
        viewOffset_.x   = mousePos.x - static_cast<int>((mousePos.x - viewOffset_.x) * factor);
        viewOffset_.y   = mousePos.y - static_cast<int>((mousePos.y - viewOffset_.y) * factor);

        Refresh();
    }

    void OnRightDown(wxMouseEvent& event)
    {
        lastMousePos_ = event.GetPosition();
    }

    void OnMouseMove(wxMouseEvent& event)
    {
        if (event.RightIsDown())
        {
            wxPoint currentPos = event.GetPosition();
            viewOffset_ += (currentPos - lastMousePos_);
            lastMousePos_ = currentPos;
            Refresh();
        }
    }

    void UpdateHierarchy(GameEngine::Animation::Joint& joint, const mat4& parentTransform)
    {
        joint.worldTransform = (joint.ignoreParentRotation or false) ? joint.transform : parentTransform * joint.transform;

        for (GameEngine::Animation::Joint& child : joint.children)
        {
            UpdateHierarchy(child, joint.worldTransform);
        }
    }
};