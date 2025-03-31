#include "Utility/COMInitializer.h"

namespace Utility
{
    /**
     * @brief Constructor - Initializes COM for the current thread.
     *
     * Uses CoInitializeEx with the specified initialization flags (e.g., COINIT_MULTITHREADED).
     * If initialization fails, throws a std::runtime_error.
     *
     * @param coinitFlags Initialization flags. Default is COINIT_MULTITHREADED.
     * @throws std::runtime_error if COM initialization fails.
     */
    COMInitializer::COMInitializer(DWORD coinitFlags)
    {
        HRESULT hr = CoInitializeEx(nullptr, coinitFlags);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to initialize COM.");
        }
        m_initialized = true;
    }

    /**
     * @brief Destructor - Uninitializes COM if it was previously initialized.
     *
     * Automatically calls CoUninitialize, ensuring clean shutdown of COM on the thread.
     */
    COMInitializer::~COMInitializer()
    {
        if (m_initialized)
        {
            CoUninitialize();
        }
    }

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of the COM initialization state from another instance.
     * The source object is marked as uninitialized to prevent double uninitialization.
     *
     * @param other The COMInitializer object to move from.
     */
    COMInitializer::COMInitializer(COMInitializer&& other) noexcept
        : m_initialized(other.m_initialized)
    {
        other.m_initialized = false;
    }

    /**
     * @brief Move assignment operator.
     *
     * Safely transfers the COM initialization state from one instance to another.
     * Ensures that the current instance's COM is uninitialized before taking over.
     *
     * @param other The COMInitializer object to move from.
     * @return Reference to this object.
     */
    COMInitializer& COMInitializer::operator=(COMInitializer&& other) noexcept
    {
        if (this != &other)
        {
            if (m_initialized)
                CoUninitialize();

            m_initialized = other.m_initialized;
            other.m_initialized = false;
        }
        return *this;
    }
}
