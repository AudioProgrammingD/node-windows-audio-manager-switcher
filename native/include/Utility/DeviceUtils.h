#pragma once

#include <string>
#include <mmdeviceapi.h>
#include "Utility/DeviceFormatInfo.h"

namespace Utility
{
    /**
     * @brief Retrieves the friendly name of an audio device.
     *
     * This function uses the device's property store to extract the user-visible name,
     * such as "Speakers (Realtek Audio)" or "Headphones".
     *
     * @param device Pointer to a valid IMMDevice.
     * @return std::wstring Friendly name of the device, or "Unknown" if retrieval fails.
     */
    std::wstring GetDeviceFriendlyName(IMMDevice *device);

    /**
     * @brief Retrieves audio format information (bit depth, sample rate, channels, etc.) for a device.
     *
     * Internally uses IAudioClient::GetMixFormat to get the shared-mode default audio format.
     *
     * @param device Pointer to a valid IMMDevice.
     * @return DeviceFormatInfo Struct containing audio format fields.
     *         Check `valid` field to confirm if data was successfully retrieved.
     */
    DeviceFormatInfo GetDeviceFormatInfo(IMMDevice *device);

    /**
     * @brief Retrieves the system's current default audio playback (render) device.
     *
     * This corresponds to the "default device" shown in Windows sound settings (used for system audio).
     * Caller is responsible for releasing the returned pointer using `SafeRelease()`.
     *
     * @return IMMDevice* Pointer to the default device. Returns nullptr on failure.
     */
    IMMDevice *GetDefaultAudioPlaybackDevice();

    /**
     * @brief Mutes or unmutes the default audio playback device.
     *
     * This uses IAudioEndpointVolume COM interface to control the system volume mute state.
     *
     * @param mute True to mute, false to unmute.
     * @return true if successful, false otherwise.
     */
    bool SetDefaultPlaybackDeviceMute(bool mute);

    /**
     * @brief Mutes or unmutes the given audio playback device.
     *
     * This uses the IAudioEndpointVolume COM interface to mute the specified device.
     *
     * @param device Pointer to the IMMDevice to mute/unmute.
     * @param mute True to mute, false to unmute.
     * @return true if successful, false otherwise.
     */
    bool MuteDevice(IMMDevice *device, bool mute);
}
