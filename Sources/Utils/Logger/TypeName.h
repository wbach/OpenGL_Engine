#pragma once
#include <string>

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

template <typename T>
std::string typeName()
{
#ifdef _MSC_VER
    return typeid(T).name();
#else
    int status;
    std::string tname    = typeid(T).name();
    char* demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
    if (status == 0)
    {
        tname = demangled_name;
        std::free(demangled_name);
    }
    return tname;
#endif
}

template <typename T>
std::string typeName(const T&)
{
    return typeName<T>();
}