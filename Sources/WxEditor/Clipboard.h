#pragma once
#include <SingleTon.h>
#include <TreeNode.h>

#include <variant>

struct ComponentData
{
    TreeNode node;
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
        m_buffer = std::move(content);
    }

    template <typename T>
    const T* GetAs() const
    {
        return std::get_if<T>(&m_buffer);
    }

    bool IsEmpty() const
    {
        return std::holds_alternative<std::monostate>(m_buffer);
    }

    void Clear()
    {
        m_buffer = std::monostate{};
    }

private:
    ClipboardContent m_buffer;
};

#define Clipboard SingleTon<ClipboardImpl>::Get()