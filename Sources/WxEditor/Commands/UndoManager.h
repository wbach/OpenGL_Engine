#pragma once
#include <cstddef>
#include <memory>
#include <stack>

#include "Command.h"
#include "Logger/Log.h"
#include "SingleTon.h"
#include "TreeNode.h"

class UndoManagerImpl
{
public:
    using CommandPtr = std::unique_ptr<Command>;

    void Push(CommandPtr cmd)
    {
        undoStack.push(std::move(cmd));
        redoStack = {};
    }

    void Undo()
    {
        if (undoStack.empty())
        {
            LOG_DEBUG << "Stack is empty";
            return;
        }

        CommandPtr cmd = std::move(undoStack.top());
        undoStack.pop();

        cmd->undo();
        redoStack.push(std::move(cmd));
    }

    void Redo()
    {
        if (redoStack.empty())
        {
            LOG_DEBUG << "Stack is empty";
            return;
        }

        CommandPtr cmd = std::move(redoStack.top());
        redoStack.pop();

        cmd->redo();
        undoStack.push(std::move(cmd));
    }

    void Clear()
    {
        undoStack = {};
        redoStack = {};
    }

    size_t GetUndoStackSize()
    {
        return undoStack.size();
    }

    size_t GetRedoStackSize()
    {
        return redoStack.size();
    }

private:
    std::stack<CommandPtr> undoStack;
    std::stack<CommandPtr> redoStack;

    std::unordered_map<IdType, TreeNode> objectCache;
};

using UndoManager = SingleTon<UndoManagerImpl>;