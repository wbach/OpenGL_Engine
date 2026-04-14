#pragma once

class Params
{
public:
    virtual ~Params() = default;

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
