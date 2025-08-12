#include "Theme.h"

// clang-format off
const WxTheme LIGHT_THEME{
    .bg{225, 225, 225},
    .fg{180, 180, 180},
    .txtBg{40, 40, 40}
};

const WxTheme DARK_THEME{
    .bg{30, 30, 30},
    .fg{220, 220, 220},
    .txtBg{50, 50, 50}
};
// clang-format on

WxTheme OS_DEFAULT_THEME;

void ApplyTheme(wxWindow &window, const WxTheme &theme)
{
    if (auto txt = dynamic_cast<wxTextCtrl *>(&window))
    {
        txt->SetBackgroundColour(theme.txtBg);
        txt->SetForegroundColour(theme.fg);
    }
    else if (auto choice = dynamic_cast<wxChoice *>(&window))
    {
        choice->SetBackgroundColour(theme.txtBg);
        choice->SetForegroundColour(theme.fg);
    }
    else if (auto combo = dynamic_cast<wxComboBox *>(&window))
    {
        combo->SetBackgroundColour(theme.txtBg);
        combo->SetForegroundColour(theme.fg);
    }
    else if (auto list = dynamic_cast<wxListBox *>(&window))
    {
        list->SetBackgroundColour(theme.txtBg);
        list->SetForegroundColour(theme.fg);
        // Jeśli chcesz, możesz też ustawić kolory zaznaczenia:
        // list->SetSelectionBackground(wxColour(100, 100, 100));
        // list->SetSelectionForeground(wxColour(255, 255, 255));
    }
    else
    {
        window.SetBackgroundColour(theme.bg);
        window.SetForegroundColour(theme.fg);
    }

    window.Refresh();

    for (wxWindow *child : window.GetChildren())
    {
        ApplyTheme(*child, theme);
    }
}


void SaveOsTheme(wxWindow &window)
{
    if (auto txt = dynamic_cast<wxTextCtrl *>(&window))
    {
        OS_DEFAULT_THEME.txtBg = txt->GetBackgroundColour();
        OS_DEFAULT_THEME.fg    = txt->GetForegroundColour();
    }
    else
    {
        OS_DEFAULT_THEME.bg = window.GetBackgroundColour();
        OS_DEFAULT_THEME.fg = window.GetForegroundColour();
    }

    for (wxWindow *child : window.GetChildren())
    {
        SaveOsTheme(*child);
    }
}
