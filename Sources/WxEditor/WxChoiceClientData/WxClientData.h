#pragma once
#include <wx/wx.h>

template <class T>
class WxClientData : public wxClientData
{
public:
    WxClientData(const T& value)
        : value(value)
    {
    }
    const T& GetValue() const
    {
        return value;
    }

private:
    T value;
};