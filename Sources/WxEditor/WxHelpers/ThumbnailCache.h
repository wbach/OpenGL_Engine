#pragma once
#include <wx/wx.h>

WX_DECLARE_STRING_HASH_MAP(wxBitmap, BitmapCache);

class ThumbnailCache
{
public:
    static ThumbnailCache& Get()
    {
        static ThumbnailCache instance;
        return instance;
    }

    ThumbnailCache()  = default;
    ~ThumbnailCache() = default;

    ThumbnailCache(const ThumbnailCache&)            = delete;
    ThumbnailCache& operator=(const ThumbnailCache&) = delete;

    wxBitmap GetThumbnail(const wxString& input, int thumbSize = 64);

private:
    bool IsImageExt(const wxString&);

private:
    BitmapCache cache;
};
