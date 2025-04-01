/**
 * @file addon.cpp
 * @author sameerbk201
 * @brief Native Node.js addon to list Windows audio playback devices using C++ and N-API.
 *
 * This module wraps a C++ library that interfaces with Windows Core Audio APIs,
 * providing an easy-to-use method `listDevices()` accessible from Node.js.
 */

#include <napi.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include "AudioSwitcher/AudioSwitcher.h"
#include "Utility/COMInitializer.h"
#include <mmdeviceapi.h>
#include "Utility/DeviceUtils.h"
#include <Utility/SafeRelease.h>
using namespace AudioSwitcher;
using namespace Utility;

/**
 * @brief Converts a wide Unicode string (std::wstring) to a UTF-8 encoded std::string.
 *
 * Uses Windows API `WideCharToMultiByte` to perform the conversion.
 *
 * @param wstr The wide-character string to convert.
 * @return std::string UTF-8 encoded version of the input.
 */
std::string WStringToUtf8(const std::wstring &wstr)
{
    if (wstr.empty())
        return {};

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()),
        nullptr, 0, nullptr, nullptr);

    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()),
        &result[0], sizeNeeded, nullptr, nullptr);

    return result;
}

/**
 * @file    StringConversion.h
 * @brief   Contains UTF-8 to wide string conversion utilities.
 */

/**
 * @brief   Converts a UTF-8 encoded string to a wide character string.
 *
 * @details This function converts a UTF-8 encoded std::string to a std::wstring
 *          using Windows API MultiByteToWideChar function with CP_UTF8 code page.
 *          The function handles empty strings efficiently by returning an empty wstring.
 *
 * @param   str The input UTF-8 encoded string to convert.
 *
 * @return  std::wstring The converted wide character string.
 *
 * @note    This function is Windows-specific as it uses the Windows API.
 * @warning The input string must be valid UTF-8 encoded text.
 *
 * @example
 * std::string utf8Str = u8;
 * std::wstring wideStr = Utf8ToWString(utf8Str);
 * // wideStr now contains the wide character version
 */
std::wstring Utf8ToWString(const std::string &str)
{
    if (str.empty())
        return {};

    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

/**
 * @brief   Retrieves a list of available audio playback devices with default status.
 *
 * @details This N-API function enumerates all active audio output devices and returns them
 *          as an array of JavaScript objects. Each device object contains:
 *          - A human-readable display name
 *          - A unique system identifier for device operations
 *          - A boolean flag indicating if it's the default playback device
 *
 *          The function:
 *          1. Initializes COM (required for Windows Core Audio API)
 *          2. Retrieves the current default playback device ID
 *          3. Retrieves device list through AudioManager
 *          4. Converts device information to JavaScript objects
 *          5. Compares each device ID with default ID to set isDefault flag
 *          6. Returns array of device objects
 *
 * @param   info Napi::CallbackInfo (unused parameters)
 * @return  Napi::Array Array of device objects in format:
 *              `{ name: string, id: string, isDefault: boolean }`
 * @throws  Napi::TypeError When:
 *              - COM initialization fails
 *              - Device enumeration fails
 *              - String conversion fails
 *              - Default device detection fails
 *
 * @note    Requires COM initialization (handled automatically via RAII)
 * @warning Device IDs are system-specific and should be treated as opaque strings
 * @remark  Uses Windows Core Audio API through AudioManager wrapper
 * @see     SetDefaultDevice For using the returned device IDs
 *
 * @example
 * // JavaScript usage:
 * const devices = listDevices();
 * devices.forEach(device => {
 *   const defaultStatus = device.isDefault ? "(Default)" : "";
 *   console.log(`Device: ${device.name} ${defaultStatus} (ID: ${device.id})`);
 * });
 */
Napi::Value ListDevices(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        // Initialize COM for audio device operations
        // std::cout << "[C++] Initializing COM..." << std::endl;
        COMInitializer com;

        // Get current default device ID to compare against all devices
        IMMDevice *defaultDevice = Utility::GetDefaultAudioPlaybackDevice();
        std::wstring defaultIdW = L"";
        if (defaultDevice)
        {
            LPWSTR buffer = nullptr;
            HRESULT hr = defaultDevice->GetId(&buffer);
            if (SUCCEEDED(hr) && buffer)
            {
                defaultIdW = buffer;
                CoTaskMemFree(buffer);
            }
            Utility::SafeRelease(defaultDevice);
        }

        std::string defaultIdUtf8 = WStringToUtf8(defaultIdW);

        // std::cout << "[C++] COM initialized." << std::endl;
        // Retrieve system audio devices
        auto devices = AudioManager::listOutputDevices();
        // std::cout << "[C++] Devices found: " << devices.size() << std::endl;
        // Create JavaScript array for results
        Napi::Array result = Napi::Array::New(env, devices.size());
        // Convert each device to JavaScript object
        for (size_t i = 0; i < devices.size(); ++i)
        {
            // Convert wide strings to UTF-8 for JavaScript
            std::string name = WStringToUtf8(devices[i].name);
            std::string id = WStringToUtf8(devices[i].id);

            // std::cout << "[C++] Device " << i << ": " << name << std::endl;
            // Create device object with name, ID and default status
            Napi::Object obj = Napi::Object::New(env);
            obj.Set("name", Napi::String::New(env, name));
            obj.Set("id", Napi::String::New(env, id));
            obj.Set("isDefault", Napi::Boolean::New(env, id == defaultIdUtf8));

            result.Set(i, obj);
        }

        return result;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[C++] Exception: " << ex.what() << std::endl;
        Napi::TypeError::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

/**
 * @brief   Sets the mute state for the default audio playback device.
 *
 * @details This N-API function controls the mute state of the system's default
 *          audio playback device. It provides a programmatic way to mute or unmute
 *          the primary audio output device.
 *
 *          The function:
 *          1. Validates input parameters
 *          2. Initializes COM (required for Windows Core Audio API)
 *          3. Attempts to set the mute state through Utility functions
 *          4. Returns operation success status
 *
 * @param   info Napi::CallbackInfo containing:
 *              - A single boolean parameter (true = mute, false = unmute)
 * @return  Napi::Boolean indicating operation success (true = success, false = failure)
 * @throws  Napi::TypeError When:
 *              - Invalid number of arguments provided
 *              - Argument is not a boolean
 * @throws  Napi::Error When:
 *              - COM initialization fails
 *              - Device mute operation fails
 *
 * @note    Requires COM initialization (handled automatically via RAII)
 * @warning This affects the system's default playback device - use with caution
 * @remark  Uses Windows Core Audio API through Utility wrapper functions
 * @see     GetDefaultPlaybackMute For retrieving current mute state
 *
 * @example
 * // JavaScript usage:
 * // Mute the default playback device
 * const success = setDefaultPlaybackMute(true);
 * console.log(`Mute operation ${success ? "succeeded" : "failed"}`);
 *
 * // Unmute the default playback device
 * setDefaultPlaybackMute(false);
 */
Napi::Value SetDefaultPlaybackMute(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // Validate exactly one boolean argument is provided
    if (info.Length() != 1 || !info[0].IsBoolean())
    {
        Napi::TypeError::New(env, "Expected one boolean argument (true=mute, false=unmute)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    // Extract the mute state from JavaScript argument
    bool mute = info[0].As<Napi::Boolean>().Value();

    try
    {
        // Initialize COM for audio device operations
        COMInitializer com;

        // Attempt to set mute state and return operation result
        bool success = Utility::SetDefaultPlaybackDeviceMute(mute);
        return Napi::Boolean::New(env, success);
    }
    catch (...)
    {
        // Handle any errors during mute operation
        Napi::Error::New(env, "Failed to set mute state for default playback device")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
}

/**
 * @brief   Mutes or unmutes a specific audio playback device by its ID.
 *
 * @details This N-API function provides direct control over individual audio devices by:
 *          1. Accepting a device ID (from ListDevices()) and desired mute state
 *          2. Directly accessing the audio endpoint device through Windows Core Audio API
 *          3. Applying the mute state change to the specified device
 *
 *          The operation performs these steps:
 *          1. Validates input parameters
 *          2. Initializes COM (required for Windows Core Audio API)
 *          3. Creates device enumerator instance
 *          4. Gets device interface directly by ID
 *          5. Applies mute state through Utility functions
 *          6. Returns operation success status
 *
 * @param   info Napi::CallbackInfo containing:
 *              - deviceId: string (Device identifier from ListDevices())
 *              - mute: boolean (true = mute, false = unmute)
 * @return  Napi::Boolean indicating operation result:
 *              - true: Mute operation succeeded
 *              - false: Device not found or operation failed
 * @throws  Napi::TypeError When:
 *              - Invalid number of arguments provided
 *              - First argument is not a string
 *              - Second argument is not a boolean
 * @throws  Napi::Error When:
 *              - COM initialization fails
 *              - Device access operation fails
 *
 * @note    Requires COM initialization (handled automatically via RAII)
 * @warning Device IDs must be exact matches (case-sensitive)
 * @remark  Uses direct device access via IMMDeviceEnumerator for better performance
 *          compared to enumeration approach
 * @see     ListDevices() For obtaining valid device IDs
 * @see     SetDefaultPlaybackMute() For controlling default device mute state
 *
 * @example
 * // JavaScript usage:
 * // First get device list
 * const devices = listDevices();
 *
 * // Mute specific device (using first device ID as example)
 * if (devices.length > 0) {
 *   const success = muteDeviceById(devices[0].id, true);
 *   console.log(`Mute operation ${success ? "succeeded" : "failed"}`);
 * }
 */
Napi::Value MuteDeviceById(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // Validate exactly two arguments: string deviceId and boolean mute state
    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsBoolean())
    {
        Napi::TypeError::New(env, "Expected arguments: (string deviceId, boolean mute)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    // Extract device ID (UTF-8) and desired mute state from arguments
    std::string idUtf8 = info[0].As<Napi::String>().Utf8Value();
    bool mute = info[1].As<Napi::Boolean>().Value();

    // Convert device ID to wide string (Windows API requirement)
    std::wstring deviceId(idUtf8.begin(), idUtf8.end());

    try
    {
        // Initialize COM for audio device operations (RAII ensures proper cleanup)
        COMInitializer com;

        // Create device enumerator instance
        IMMDeviceEnumerator *enumerator = nullptr;
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            nullptr,
            CLSCTX_ALL,
            __uuidof(IMMDeviceEnumerator),
            (void **)&enumerator);

        // Return false if enumerator creation failed
        if (FAILED(hr) || !enumerator)
        {
            return Napi::Boolean::New(env, false);
        }

        // Get specific device interface by ID
        IMMDevice *device = nullptr;
        hr = enumerator->GetDevice(deviceId.c_str(), &device);
        Utility::SafeRelease(enumerator); // Clean up enumerator immediately after use

        // Return false if device access failed
        if (FAILED(hr) || !device)
        {
            return Napi::Boolean::New(env, false);
        }

        // Attempt mute operation and clean up device reference
        bool result = Utility::MuteDevice(device, mute);
        Utility::SafeRelease(device);

        return Napi::Boolean::New(env, result);
    }
    catch (...)
    {
        // Handle any unexpected errors during device operation
        Napi::Error::New(env, "Failed to mute device - system error occurred")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
}

/**
 * @file    AudioDeviceManager.h
 * @brief   N-API interface for managing audio output devices.
 */

/**
 * @brief   Sets the default audio output device using the specified device ID.
 *
 * @details This N-API function sets the default audio output device by its ID string.
 *          It performs the following operations:
 *          1. Validates input parameters
 *          2. Converts UTF-8 string to wide string
 *          3. Initializes COM (for Windows audio APIs)
 *          4. Verifies the device exists in available output devices
 *          5. Attempts to set the device as default
 *
 * @param   info Napi::CallbackInfo containing:
 *              - args[0]: Device ID string (UTF-8 encoded)
 *
 * @return  Napi::Value Either:
 *              - Napi::Boolean indicating success (true) or failure (false)
 *              - Napi::Null if invalid arguments or exception occurs
 *
 * @throws  Napi::TypeError When:
 *              - No arguments provided
 *              - First argument is not a string
 *              - COM initialization fails
 *              - Audio device operations fail
 *
 * @note    This function requires COM initialization (handled internally)
 * @warning Device ID must match exactly with system-registered IDs
 * @remark  Uses Windows Core Audio API through AudioManager wrapper
 *
 * @example
 * // JavaScript usage:
 * const success = setDefaultDevice('{0.0.0.00000000}.{guid-string}');
 * console.log('Device set:', success);
 */
Napi::Value SetDefaultDevice(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // Validate input parameters
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Device ID string expected").ThrowAsJavaScriptException();
        return env.Null();
    }
    // Convert UTF-8 JavaScript string to wide string
    std::string deviceIdUtf8 = info[0].As<Napi::String>();
    std::wstring deviceIdW(deviceIdUtf8.begin(), deviceIdUtf8.end());

    try
    {
        // Initialize COM for audio device operations
        COMInitializer com;
        // Get available output devices
        auto devices = AudioManager::listOutputDevices();
        // Verify device exists
        auto it = std::find_if(devices.begin(), devices.end(), [&](const AudioDevice &dev)
                               { return dev.id == deviceIdW; });

        if (it == devices.end())
        {
            std::wcerr << L"[C++] Device ID not found in list." << std::endl;
            return Napi::Boolean::New(env, false);
        }
        // Attempt to set default device
        bool result = AudioManager::setDefaultOutputDevice(deviceIdW);
        std::wcout << L"[C++] Set default result: " << (result ? L"Success" : L"Fail") << std::endl;

        return Napi::Boolean::New(env, result);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[C++] Exception: " << ex.what() << std::endl;
        Napi::TypeError::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

/**
 * @brief Initializes and exports native C++ functions to JavaScript.
 *
 * This function binds the native methods so they can be accessed in JS via:
 * ```js
 * const audio = require('node-windows-audio-manager');
 * audio.listDevices();
 * audio.setDefaultDevice("deviceId");
 * audio.setDefaultPlaybackMute(true);
 * audio.muteDeviceById("deviceId", true);
 * ```
 *
 * @param env The environment context
 * @param exports The JS object to which native functions are attached
 * @return Napi::Object with bound native methods
 */
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("listDevices", Napi::Function::New(env, ListDevices));
    exports.Set("setDefaultDevice", Napi::Function::New(env, SetDefaultDevice));
    exports.Set("setDefaultPlaybackMute", Napi::Function::New(env, SetDefaultPlaybackMute));
    exports.Set("muteDeviceById", Napi::Function::New(env, MuteDeviceById));
    return exports;
}

// Macro that defines the entry point for this native module
NODE_API_MODULE(addon, Init)
