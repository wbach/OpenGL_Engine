#pragma once

class Params
{
public:
    Params()  = default;
    virtual ~Params() = default;

    Params(const Params& other)
        : dirty(true)
    {
    }

    Params& operator=(const Params& other)
    {
        if (this == &other)
            return *this;

        dirty = true;
        return *this;
    }

    bool isDirty() const
    {
        return dirty;
    }
    void markAsDirty()
    {
        dirty = true;
    }
    void clearDirty()
    {
        dirty = false;
    }
    bool* getDirtyPtr()
    {
        return &dirty;
    }

protected:
    bool dirty = true;
};
