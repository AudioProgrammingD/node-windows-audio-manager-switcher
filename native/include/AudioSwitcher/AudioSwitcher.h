#pragma once

#include <string>
#include <vector>
#include <mmdeviceapi.h> // Required for IMMDevice*

namespace AudioSwitcher
{
    /**
     * @brief Struct representing an audio output device.
     */
    struct AudioDevice
    {
        std::wstring id;             ///< The unique ID of the audio device (used by the system).
        std::wstring name;           ///< Friendly name shown to the user (e.g., "Speakers", "Headset").
        IMMDevice *device = nullptr; ///< Pointer to the actual device object (optional for advanced use).
        // Explicit default constructor to fix the issue
        AudioDevice() = default;
        // Optional: Destructor to safely release if ever needed
        ~AudioDevice()
        {
            if (device)
                device->Release();
        }

        // Optional: Copy constructor/operator deleted to avoid double-release
        AudioDevice(const AudioDevice &) = delete;
        AudioDevice &operator=(const AudioDevice &) = delete;

        // Allow move semantics (optional and useful)
        AudioDevice(AudioDevice &&other) noexcept
        {
            id = std::move(other.id);
            name = std::move(other.name);
            device = other.device;
            other.device = nullptr;
        }

        AudioDevice &operator=(AudioDevice &&other) noexcept
        {
            if (this != &other)
            {
                if (device)
                    device->Release();
                id = std::move(other.id);
                name = std::move(other.name);
                device = other.device;
                other.device = nullptr;
            }
            return *this;
        }
    };

    /**
     * @brief Main class responsible for managing audio output devices.
     */
    class AudioManager
    {
    public:
        /**
         * @brief Lists all active audio output devices on the system.
         *
         * @return std::vector<AudioDevice> List of devices with ID, name, and raw pointer.
         */
        static std::vector<AudioDevice> listOutputDevices();

        /**
         * @brief Sets the given device as the default playback device.
         *
         * @param deviceId The device ID string.
         * @return true if successful, false otherwise.
         */
        static bool setDefaultOutputDevice(const std::wstring &deviceId);
    };

} // namespace AudioSwitcher
