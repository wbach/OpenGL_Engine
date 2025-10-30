#include "ThumbnailCache.h"

#include <GameEngine/Resources/File.h>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/hashmap.h>

wxBitmap ThumbnailCache::GetThumbnail(const wxString& input, int thumbSize)
{
    if (input.IsEmpty())
    {
        return wxNullBitmap;
    }

    GameEngine::File file(input);
    wxString path = file.GetAbsolutePath().string();

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

    // Dodajemy do cache tylko jesli mamy bitmape
    if (bmp.IsOk())
    {
        cache[path] = bmp;
    }

    return bmp;
}
bool ThumbnailCache::IsImageExt(const wxString& ext)
{
    return ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "tga";
}
