#pragma once
#include <wx/wx.h>

struct WxTheme
{
    wxColour bg{30, 30, 30};    // ciemne tlo
    wxColour fg{220, 220, 220}; // jasny tekst
    wxColour txtBg{50, 50, 50}; // tlo pol tekstowych
};

extern const WxTheme LIGHT_THEME;
extern const WxTheme DARK_THEME;
extern WxTheme OS_DEFAULT_THEME;

void ApplyTheme(wxWindow&, const WxTheme& = DARK_THEME);
void SaveOsTheme(wxWindow&);
