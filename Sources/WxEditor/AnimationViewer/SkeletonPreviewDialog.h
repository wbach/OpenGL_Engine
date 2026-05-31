#pragma once
#include <GameEngine/Animations/Skeleton.h>
#include <wx/dcbuffer.h>
#include <wx/wx.h>

#include <glm/gtc/matrix_access.hpp>

class SkeletonPreviewDialog : public wxDialog
{
public:
    enum class Mode
    {
        SHOW_ONLY,
        SINGLE_SELECTION
    };
    SkeletonPreviewDialog(wxWindow*, const GameEngine::Animation::Skeleton&, const std::string&, Mode = Mode::SHOW_ONLY);

    const std::string& GetSelectedName() const;
    GameEngine::Animation::JointId GetSelectedId() const;
    bool WasJointSelected() const;

private:
    void OnPaint(wxPaintEvent&);
    void DrawJoint(wxDC&, const GameEngine::Animation::Joint&);
    wxPoint WorldToScreen(const mat4&) const;
    void OnMouseWheel(wxMouseEvent&);
    void OnRightDown(wxMouseEvent&);
    void OnMouseMove(wxMouseEvent&);
    void UpdateHierarchy(GameEngine::Animation::Joint&, const mat4&);
    const GameEngine::Animation::Joint* FindJointAt(const GameEngine::Animation::Joint&, wxPoint);
    void TrySelectSingle(wxMouseEvent&);
    std::string BuildHelpText() const;
    void OnLeftDown(wxMouseEvent&);

private:
    Mode mode_;
    GameEngine::Animation::Joint skeletonCopy_;
    float scale_ = 300.0f;
    wxPoint viewOffset_;
    wxPoint lastMousePos_;
    std::string notificationText = "";
    wxLongLong notificationExpiry = 0;

    std::string selectedName_                  = "";
    GameEngine::Animation::JointId selectedId_ = 0;
    bool jointSelected_                        = false;
};
