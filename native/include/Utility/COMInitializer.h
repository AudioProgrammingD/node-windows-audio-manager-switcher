#pragma once

#include <objbase.h>
#include <stdexcept>

namespace Utility
{
    /**
     * @brief RAII-style COM initializer for Windows.
     * 
     * Automatically calls CoInitializeEx in constructor and CoUninitialize in destructor.
     * This ensures proper COM setup and cleanup for any thread using COM.
     */
    class COMInitializer
    {
    public:
        /**
         * @brief Initializes COM for the current thread.
         * @param coinitFlags Flags for COM initialization (e.g., COINIT_MULTITHREADED).
         * @throws std::runtime_error if COM initialization fails.
         */
        explicit COMInitializer(DWORD coinitFlags = COINIT_MULTITHREADED);

        /// Destructor that calls CoUninitialize if initialized.
        ~COMInitializer();

        // Delete copy constructor/assignment to avoid unintended copies
        COMInitializer(const COMInitializer&) = delete;
        COMInitializer& operator=(const COMInitializer&) = delete;

        // Allow move operations
        COMInitializer(COMInitializer&& other) noexcept;
        COMInitializer& operator=(COMInitializer&& other) noexcept;

    private:
        bool m_initialized = false;
    };
}
