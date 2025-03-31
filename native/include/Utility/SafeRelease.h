#pragma once



#include <windows.h>

namespace Utility
{
    /**
     * @brief Releases a COM object safely and sets the pointer to nullptr.
     */
    template <typename T>
    void SafeRelease(T *&ptr)
    {
        if (ptr)
        {
            ptr->Release();
            ptr = nullptr;
        }
    }
}
