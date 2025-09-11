#pragma once

#include <Logger/Log.h>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/hashmap.h>
#include <wx/wx.h>
#include <GameEngine/Resources/File.h>

WX_DECLARE_STRING_HASH_MAP(wxBitmap, BitmapCache);

class ThumbnailCache
{
public:
    static ThumbnailCache& Get()
    {
        static ThumbnailCache instance;
        return instance;
    }

    wxBitmap GetThumbnail(const wxString& input, int thumbSize = 64)
    {
        if (input.IsEmpty())
        {
            return wxNullBitmap;
        }

        GameEngine::File file(input);
        wxString path = file.GetAbsolutePath();

        auto it = cache.find(path);
        if (it != cache.end())
        {
            return it->second;
        }

        wxFileName fn(path);
        wxString ext = fn.GetExt().Lower();
        wxBitmap bmp = wxNullBitmap;

        if (IsImageExt(ext))
        {
            wxLogNull noLog;
            wxImage img(fn.GetFullPath(), wxBITMAP_TYPE_ANY);
            if (img.IsOk())
            {
                double scale = std::min(double(thumbSize) / img.GetWidth(), double(thumbSize) / img.GetHeight());
                int nw       = std::max(1, int(img.GetWidth() * scale));
                int nh       = std::max(1, int(img.GetHeight() * scale));
                img          = img.Scale(nw, nh, wxIMAGE_QUALITY_HIGH);
                if (!img.HasAlpha())
                    img.InitAlpha();
                bmp = wxBitmap(img);
            }
        }

        // Dodajemy do cache tylko jeśli mamy bitmapę
        if (bmp.IsOk())
        {
            cache[path] = bmp;
        }

        return bmp;
    }

private:
    BitmapCache cache;

    ThumbnailCache()  = default;
    ~ThumbnailCache() = default;

    ThumbnailCache(const ThumbnailCache&)            = delete;
    ThumbnailCache& operator=(const ThumbnailCache&) = delete;

    bool IsImageExt(const wxString& ext)
    {
        return ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "tga";
    }
};
