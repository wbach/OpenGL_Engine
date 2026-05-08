#pragma once
#include <SingleTon.h>
#include <TreeNode.h>

#include <sstream>
#include <variant>

#include "Utils.h"

struct ComponentData
{
    TreeNode node;
    std::string typeName;
};

using ClipboardContent = std::variant<std::monostate, std::string, ComponentData>;

class ClipboardImpl
{
public:
    static ClipboardImpl& Get()
    {
        static ClipboardImpl instance;
        return instance;
    }

    void SetContent(ClipboardContent content)
    {
        buffer_ = std::move(content);
    }

    template <typename T>
    const T* GetAs() const
    {
        return std::get_if<T>(&buffer_);
    }

    bool IsEmpty() const
    {
        return std::holds_alternative<std::monostate>(buffer_);
    }

    void Clear()
    {
        buffer_ = std::monostate{};
    }

private:
    ClipboardContent buffer_;
};

#define Clipboard SingleTon<ClipboardImpl>::Get()