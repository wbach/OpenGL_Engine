#pragma once

class Command
{
public:
    virtual ~Command()  = default;
    virtual void redo() = 0;
    virtual void undo() = 0;
};