#pragma once



#include <cstdint>

namespace Utility
{
    /**
     * @brief Represents audio format info for a playback device.
     */
    struct DeviceFormatInfo
    {
        uint16_t bitDepth = 0;
        uint16_t channels = 0;
        uint16_t blockAlign = 0;
        uint32_t sampleRate = 0;
        bool valid = false; ///< Indicates if data is valid (device was readable)
    };
}
