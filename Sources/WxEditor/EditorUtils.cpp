#include "EditorUitls.h"

wxImage TrimMargins(const wxImage& img)
{
    if (!img.IsOk())
        return img;

    if (!img.HasAlpha())
        return img; // bez alfy nie wiemy co jest tłem – można dodać heurystykę

    int minX = img.GetWidth(), minY = img.GetHeight();
    int maxX = 0, maxY = 0;

    for (int y = 0; y < img.GetHeight(); ++y)
    {
        for (int x = 0; x < img.GetWidth(); ++x)
        {
            if (img.GetAlpha(x, y) > 0) // piksel widoczny
            {
                if (x < minX) minX = x;
                if (y < minY) minY = y;
                if (x > maxX) maxX = x;
                if (y > maxY) maxY = y;
            }
        }
    }

    if (minX > maxX || minY > maxY)
        return img; // wszystko przezroczyste, nic nie tniemy

    return img.GetSubImage(wxRect(minX, minY, maxX - minX + 1, maxY - minY + 1));
}


wxImage createImage(const unsigned char* data, unsigned int len, const wxSize& size)
{
    auto newBmp = wxBitmap::NewFromPNGData(data, len);
    wxImage img = newBmp.ConvertToImage();
    img = TrimMargins(img);
    img.Rescale(size.x, size.y, wxIMAGE_QUALITY_HIGH);
    return img;
}