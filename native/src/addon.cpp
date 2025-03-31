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
 * @brief   Retrieves a list of available audio playback devices.
 *
 * @details This N-API function enumerates all active audio output devices and returns them
 *          as an array of JavaScript objects. Each device object contains:
 *          - A human-readable display name
 *          - A unique system identifier for device operations
 *
 *          The function:
 *          1. Initializes COM (required for Windows Core Audio API)
 *          2. Retrieves device list through AudioManager
 *          3. Converts device information to JavaScript objects
 *          4. Returns array of device objects
 *
 * @param   info Napi::CallbackInfo (unused parameters)
 * @return  Napi::Array Array of device objects in format:
 *              `{ name: string, id: string }`
 * @throws  Napi::TypeError When:
 *              - COM initialization fails
 *              - Device enumeration fails
 *              - String conversion fails
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
 *   console.log(`Device: ${device.name} (ID: ${device.id})`);
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
            // Create device object
            Napi::Object obj = Napi::Object::New(env);
            obj.Set("name", Napi::String::New(env, name));
            obj.Set("id", Napi::String::New(env, id));

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
 * @brief N-API Module Initialization
 *
 * Binds the native `listDevices` function to JS when the module is loaded via `require()`.
 *
 * @param env The environment that the addon is running in.
 * @param exports The object that will be exported from the module.
 * @return Napi::Object The modified exports object.
 */
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("listDevices", Napi::Function::New(env, ListDevices));
    exports.Set("setDefaultDevice", Napi::Function::New(env, SetDefaultDevice));
    return exports;
}

// Macro that defines the entry point for this native module
NODE_API_MODULE(addon, Init)
