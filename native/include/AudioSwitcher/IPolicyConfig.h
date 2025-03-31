#pragma once

// Required Windows headers
#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <propvarutil.h>
#include <functiondiscoverykeys_devpkey.h>
#include <comdef.h>

// ----------------------------------------------------------------------------
// PolicyConfig.h
// Undocumented COM-interface IPolicyConfig for setting default audio endpoint
// ----------------------------------------------------------------------------

interface DECLSPEC_UUID("f8679f50-850a-41cf-9c72-430f290290c8")
    IPolicyConfig;

class DECLSPEC_UUID("870af99c-171d-4f9e-af0d-e63df40c2bc9")
    CPolicyConfigClient;

// ----------------------------------------------------------------------------
// interface IPolicyConfig
// ----------------------------------------------------------------------------
interface IPolicyConfig : public IUnknown
{
public:
    virtual HRESULT GetMixFormat(PCWSTR, WAVEFORMATEX **) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(
        PCWSTR,
        INT,
        WAVEFORMATEX **) = 0;

    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(
        PCWSTR) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(
        PCWSTR,
        WAVEFORMATEX *,
        WAVEFORMATEX *) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(
        PCWSTR,
        INT,
        PINT64,
        PINT64) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(
        PCWSTR,
        PINT64) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetShareMode(
        PCWSTR,
        struct DeviceShareMode *) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetShareMode(
        PCWSTR,
        struct DeviceShareMode *) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(
        PCWSTR,
        const PROPERTYKEY &,
        PROPVARIANT *) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(
        PCWSTR,
        const PROPERTYKEY &,
        PROPVARIANT *) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(
        PCWSTR wszDeviceId,
        ERole eRole) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(
        PCWSTR,
        INT) = 0;
};

// ----------------------------------------------------------------------------
// Vista Compatibility (Optional Fallback Interface)
// ----------------------------------------------------------------------------

interface DECLSPEC_UUID("568b9108-44bf-40b4-9006-86afe5b5a620")
    IPolicyConfigVista;

class DECLSPEC_UUID("294935CE-F637-4E7C-A41B-AB255460B862")
    CPolicyConfigVistaClient;

interface IPolicyConfigVista : public IUnknown
{
public:
    virtual HRESULT GetMixFormat(PCWSTR, WAVEFORMATEX **) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(
        PCWSTR,
        INT,
        WAVEFORMATEX **) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(
        PCWSTR,
        WAVEFORMATEX *,
        WAVEFORMATEX *) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(
        PCWSTR,
        INT,
        PINT64,
        PINT64) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(
        PCWSTR,
        PINT64) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetShareMode(
        PCWSTR,
        struct DeviceShareMode *) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetShareMode(
        PCWSTR,
        struct DeviceShareMode *) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(
        PCWSTR,
        const PROPERTYKEY &,
        PROPVARIANT *) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(
        PCWSTR,
        const PROPERTYKEY &,
        PROPVARIANT *) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(
        PCWSTR wszDeviceId,
        ERole eRole) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(
        PCWSTR,
        INT) = 0;
};
