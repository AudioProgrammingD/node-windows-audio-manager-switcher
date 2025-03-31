#include "AudioSwitcher/AudioSwitcher.h"
#include "AudioSwitcher/IPolicyConfig.h"

#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <iostream>
#include <comdef.h>

namespace AudioSwitcher
{
    /**
     * @brief Lists all active audio playback (render) devices.
     *
     * Uses Windows Core Audio APIs to enumerate currently active render devices (like speakers, headsets, etc.).
     *
     * @return std::vector<AudioDevice> A list of devices with their IDs and friendly names.
     *
     * @throws std::runtime_error If any COM operation fails (enumeration, property access, etc.).
     */
    std::vector<AudioDevice> AudioManager::listOutputDevices()
    {
        std::vector<AudioDevice> devices;

        // Initialize COM for this thread
        // CoInitialize(nullptr);

        IMMDeviceEnumerator *pEnum = nullptr;    // Main enumerator for audio devices
        IMMDeviceCollection *pDevices = nullptr; // Holds the list of devices

        try
        {
            // Create an instance of the device enumerator
            HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                          __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
            if (FAILED(hr))
                throw std::runtime_error("[x] Failed to create device enumerator.");

            // Get all active render (playback) devices
            hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
            if (FAILED(hr))
                throw std::runtime_error("[x] Failed to enumerate audio endpoints.");

            // Get the number of playback devices
            UINT count = 0;
            hr = pDevices->GetCount(&count);
            if (FAILED(hr))
                throw std::runtime_error("[x] Failed to retrieve device count.");
            if (count == 0)
                throw std::runtime_error("[x] No output devices found.");

            // Iterate through all devices
            for (UINT i = 0; i < count; ++i)
            {
                IMMDevice *pDevice = nullptr;
                LPWSTR deviceId = nullptr;

                // Get the i-th device
                hr = pDevices->Item(i, &pDevice);
                if (FAILED(hr))
                    continue; // Skip if failed

                // Get the unique device ID (used for switching)
                hr = pDevice->GetId(&deviceId);
                if (FAILED(hr) && pDevice)
                {
                    pDevice->Release();
                    continue;
                }

                // Get the device's property store (contains friendly name)
                IPropertyStore *pStore = nullptr;
                hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
                if (FAILED(hr) && pDevice)
                {
                    pDevice->Release();
                    continue;
                }

                // Read the friendly name from the property store
                PROPVARIANT prop;
                PropVariantInit(&prop);
                hr = pStore->GetValue(PKEY_Device_FriendlyName, &prop);
                if (SUCCEEDED(hr))
                {
                    // Successfully gathered all info: add to device list
                    AudioDevice device;
                    device.id = deviceId;
                    device.name = prop.pwszVal;
                    device.device = pDevice; // Store raw pointer for future use

                    devices.push_back(std::move(device));

                    // Note: DO NOT release pDevice here — it's owned by AudioDevice
                }
                else
                {
                    // If name retrieval failed, release device manually
                    if (pDevice)
                        pDevice->Release();
                }

                PropVariantClear(&prop);
                pStore->Release();
            }

            // Clean up enumerator/collection and uninitialize COM
            pDevices->Release();
            pEnum->Release();
            // CoUninitialize();

            return devices;
        }
        catch (const std::exception &e)
        {
            // Safe release in case of exception
            if (pDevices)
                pDevices->Release();
            if (pEnum)
                pEnum->Release();
            // CoUninitialize();

            throw std::runtime_error(e.what());
        }
    }

    /**
     * @brief Sets the given audio device as the default playback device for all roles.
     *
     * This function uses the undocumented IPolicyConfig COM interface to set the default
     * audio endpoint for the following roles:
     * - eConsole (system sounds, default apps)
     * - eMultimedia (music, videos)
     * - eCommunications (Skype, Teams, etc.)
     *
     * @param deviceId The unique device ID string (from IMMDevice::GetId()).
     * @return true if the operation was successful for all roles.
     * @return false if initialization or switching failed.
     */
    bool AudioManager::setDefaultOutputDevice(const std::wstring &deviceId)
    {
        // Initialize COM for this thread
        // HRESULT hr = CoInitialize(nullptr);
        // if (FAILED(hr))
        // {
        //     std::wcerr << L"[x] Failed to initialize COM.\n";
        //     return false;
        // }

        IPolicyConfig *pPolicyConfig = nullptr;

        try
        {
            // Create an instance of the IPolicyConfig COM object
            HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL,
                                  __uuidof(IPolicyConfig), (LPVOID *)&pPolicyConfig);

            if (FAILED(hr) || !pPolicyConfig)
                throw std::runtime_error("[x] Failed to create IPolicyConfig COM object.");

            // Set the selected device as the default for all 3 roles
            HRESULT hr1 = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eConsole);
            HRESULT hr2 = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eMultimedia);
            HRESULT hr3 = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eCommunications);

            pPolicyConfig->Release();
            // CoUninitialize();

            // Return true only if all 3 roles succeeded
            return SUCCEEDED(hr1) && SUCCEEDED(hr2) && SUCCEEDED(hr3);
        }
        catch (const std::exception &e)
        {
            if (pPolicyConfig)
                pPolicyConfig->Release();
            // CoUninitialize();

            std::wcerr << L"[x] Error: " << e.what() << L"\n";
            return false;
        }
    }
} // namespace AudioSwitcher
