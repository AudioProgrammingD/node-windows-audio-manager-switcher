#include "Utility/DeviceUtils.h"
#include "Utility/SafeRelease.h"
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h> // For PKEY_Device_FriendlyName
#include <propvarutil.h>                   // For PropVariantClear
#include <audioclient.h>                   // For IAudioClient
#include <comdef.h>                        // For HRESULT error handling
#include <stdexcept>
namespace Utility
{
    /**
     * @brief Retrieves the friendly name of a given audio device.
     *
     * This function queries the device's property store to extract the user-friendly
     * name (e.g., "Speakers", "Headphones", etc.). If it fails, it returns "Unknown".
     *
     * @param device A valid IMMDevice pointer.
     * @return std::wstring The friendly device name, or "Unknown" on failure.
     */
    std::wstring GetDeviceFriendlyName(IMMDevice *device)
    {
        if (!device)
            return L"Unknown";

        IPropertyStore *pStore = nullptr;

        // Open the device's property store for reading
        HRESULT hr = device->OpenPropertyStore(STGM_READ, &pStore);
        if (FAILED(hr) || !pStore)
            return L"Unknown";

        PROPVARIANT prop;
        PropVariantInit(&prop);

        // Retrieve the friendly name property
        hr = pStore->GetValue(PKEY_Device_FriendlyName, &prop);

        std::wstring name = L"Unknown";

        if (SUCCEEDED(hr) && prop.vt == VT_LPWSTR)
        {
            // If the value is a wide string, use it
            name = prop.pwszVal;
        }

        // Always clean up
        PropVariantClear(&prop);
        // pStore->Release();
        SafeRelease(pStore);

        return name;
    }

    /**
     * @brief Retrieves basic audio format information from a playback device.
     *
     * This includes bit depth, sample rate, channel count, and block alignment.
     * It uses the IAudioClient interface to query the shared-mode mix format.
     *
     * @param device A valid IMMDevice pointer.
     * @return DeviceFormatInfo Struct containing audio format details.
     *         The `.valid` field indicates whether retrieval succeeded.
     */
    DeviceFormatInfo GetDeviceFormatInfo(IMMDevice *device)
    {
        DeviceFormatInfo info;

        if (!device)
            return info;

        IAudioClient *pAudioClient = nullptr;

        // Activate the IAudioClient interface for this device
        HRESULT hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void **)&pAudioClient);
        if (FAILED(hr) || !pAudioClient)
            return info;

        WAVEFORMATEX *pwfx = nullptr;

        // Get the mix format (shared-mode default format)
        hr = pAudioClient->GetMixFormat(&pwfx);
        if (FAILED(hr) || !pwfx)
        {
            // pAudioClient->Release();
            SafeRelease(pAudioClient);
            return info;
        }

        // Extract format values
        info.bitDepth = pwfx->wBitsPerSample;
        info.channels = pwfx->nChannels;
        info.blockAlign = pwfx->nBlockAlign;
        info.sampleRate = pwfx->nSamplesPerSec;
        info.valid = true;

        // Clean up memory allocated by GetMixFormat
        CoTaskMemFree(pwfx);
        // pAudioClient->Release();
        SafeRelease(pAudioClient);

        return info;
    }

    /**
     * @brief Retrieves the system's current default audio playback (render) device.
     *
     * This function uses the Windows Core Audio API to query the default audio endpoint
     * for the `eRender` data flow and the `eConsole` role.
     *
     * It safely initializes COM for the current thread if needed, and uninitializes it
     * after the operation. If the caller already initialized COM, this will still work safely.
     *
     * @note Caller is responsible for releasing the returned IMMDevice pointer using `SafeRelease`.
     *
     * @return IMMDevice* Pointer to the default audio playback device, or nullptr on failure.
     */
    IMMDevice *GetDefaultAudioPlaybackDevice()
    {
        // Initialize COM for this thread if not already initialized.
        // bool comInitialized = SUCCEEDED(CoInitialize(nullptr));

        IMMDeviceEnumerator *pEnum = nullptr;
        IMMDevice *pDefaultDevice = nullptr;

        // Create the device enumerator COM object.
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            nullptr,
            CLSCTX_ALL,
            __uuidof(IMMDeviceEnumerator),
            (void **)&pEnum);

        if (FAILED(hr) || !pEnum)
        {
            // if (comInitialized)
            // CoUninitialize();
            return nullptr;
        }

        // Retrieve the default audio endpoint (render device for the console role).
        hr = pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultDevice);
        SafeRelease(pEnum);

        // If retrieval fails, uninitialize COM if we initialized it and return nullptr.
        if (FAILED(hr))
        {
            // if (comInitialized)
            // CoUninitialize();
            return nullptr;
        }

        // Do not call CoUninitialize() here if we initialized COM in this function.
        // The returned pDefaultDevice requires an active COM apartment.
        // The caller is responsible for eventually calling CoUninitialize() when done.
        return pDefaultDevice;
    }

    /**
     * @brief Sets the mute state for the default audio playback device.
     *
     * This function retrieves the default audio playback device, activates its endpoint volume interface,
     * and sets the mute state according to the specified parameter. All resources are properly released
     * before returning.
     *
     * @param mute Boolean value indicating the desired mute state:
     *             - true: mute the device
     *             - false: unmute the device
     *
     * @return bool Returns true if the operation succeeded, false otherwise.
     *
     * @note The function handles proper cleanup of COM resources in all cases.
     * @warning This function should be called from a thread initialized for COM if using COM apartment threading.
     *
     * @see GetDefaultAudioPlaybackDevice()
     */
    bool SetDefaultPlaybackDeviceMute(bool mute)
    {
        // Get default audio playback device
        IMMDevice *device = GetDefaultAudioPlaybackDevice();
        if (!device)
            return false;

        // Activate endpoint volume interface
        IAudioEndpointVolume *endpointVolume = nullptr;
        HRESULT hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void **)&endpointVolume);
        if (FAILED(hr) || !endpointVolume)
        {
            device->Release();
            return false;
        }

        // Set the mute state
        hr = endpointVolume->SetMute(mute ? TRUE : FALSE, nullptr);

        // Clean up resources
        // endpointVolume->Release();
        // device->Release();
        SafeRelease(endpointVolume);
        SafeRelease(device);

        return SUCCEEDED(hr);
    }

    /**
     * @brief Mutes or unmutes a specific audio device.
     *
     * This function controls the mute state of a given audio device by activating its
     * endpoint volume interface. The function handles COM interface activation safely
     * and includes exception handling to prevent resource leaks.
     *
     * @param[in] device Pointer to the IMMDevice interface of the audio device to control.
     *                   Must not be nullptr.
     * @param[in] mute   Desired mute state:
     *                   - true: mute the device
     *                   - false: unmute the device
     *
     * @return bool      Operation status:
     *                   - true: mute operation succeeded
     *                   - false: operation failed (invalid device, COM failure, etc.)
     *
     * @note The function uses RAII-style cleanup (SafeRelease) to ensure COM interfaces
     *       are properly released in all execution paths.
     * @warning The caller must ensure the IMMDevice pointer is valid for the duration
     *          of this call. This function does not take ownership of the device pointer.
     * @warning Requires COM initialization on the calling thread.
     *
     * @see IMMDevice
     * @see IAudioEndpointVolume
     * @see SafeRelease
     */
    bool MuteDevice(IMMDevice *device, bool mute)
    {
        // Validate input parameter
        if (!device)
            return false;

        IAudioEndpointVolume *endpointVolume = nullptr;

        try
        {
            // Activate the IAudioEndpointVolume interface from the device
            HRESULT hr = device->Activate(
                __uuidof(IAudioEndpointVolume),
                CLSCTX_ALL,
                nullptr,
                reinterpret_cast<void **>(&endpointVolume));

            // Check for activation failure
            if (FAILED(hr) || !endpointVolume)
                return false;

            // Set the desired mute state (TRUE/FALSE for COM compatibility)
            hr = endpointVolume->SetMute(mute ? TRUE : FALSE, nullptr);

            // Clean up and return result
            SafeRelease(endpointVolume);
            return SUCCEEDED(hr);
        }
        catch (...)
        {
            // Handle any exceptions and clean up
            SafeRelease(endpointVolume);
            return false;
        }
    }

}
