#pragma once

#include "../CorePCH.hpp"

namespace donut::Macros
{

#define PRAGMA_DEOPTIMIZE __pragma(optimize("", off))
#define PRAGMA_REOPTIMIZE __pragma(optimize("", on))

// Macro to deallocate a pointer
#define DEALLOCATE_POINTER(ptr) \
    do { \
        if (ptr) { \
            delete ptr; \
            ptr = nullptr; \
        } \
    } while (false)

// Macro to allocate a dynamic array of pointers
#define ALLOCATE_POINTER_ARRAY(type, size) \
    new type*[size]

// Macro to deallocate a dynamic array of pointers
#define DEALLOCATE_POINTER_ARRAY(array) \
    do { \
        if (array) { \
            delete[] array; \
            array = nullptr; \
        } \
    } while (false)

// Macro to reset all elements of a dynamic array of pointers to nullptr
#define RESET_POINTER_ARRAY(array, size) \
    do { \
        for (size_t i = 0; i < size; ++i) { \
            array[i] = nullptr; \
        } \
    } while (false)
};