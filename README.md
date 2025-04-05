# 🎧 Node Windows Audio Manager

> Native Node.js addon for managing **Windows audio playback devices**  
> Powered by **C++ + Node-API (N-API)** and ships with **prebuilt binaries** for easy plug-and-play.

---

## 🚀 Features

- 🔍 List all active audio output devices (name, ID, isDefault)
- 🎚️ Set any device as the system's default playback device
- 🔇 Mute / unmute:
  - ✅ Default output device
  - ✅ Any specific device (by ID)
- ⚙️ Built with Windows Core Audio + COM API
- 💡 Prebuilt `.node` binaries — **no build tools required**

---

## 📦 Installation

```bash
npm install node-windows-audio-manager-switcher
```

✅ Works out of the box on Windows  
✅ No Visual Studio or compilation needed

> 💬 If you modify native C++ code, see [Build From Source](#prebuilt-native-addon)

---

## 🧠 Usage Examples

### 🔍 List Playback Devices

```js
const { listDevices } = require('node-windows-audio-manager-switcher');

const devices = listDevices();
devices.forEach((device, index) => {
  console.log(`${index + 1}. ${device.name} [${device.id}] Default: ${device.isDefault}`);
});
```

---

### 🎚️ Set Default Playback Device

```js
const { listDevices, setDefaultDevice } = require('node-windows-audio-manager-switcher');

const devices = listDevices();
const target = devices.find(d => d.name.includes("Speakers"));

if (target) {
  const success = setDefaultDevice(target.id);
  console.log(success ? "✅ Set as default!" : "❌ Failed to set device.");
}
```

---

### 🔇 Mute / Unmute Default Device

```js
const { setDefaultPlaybackMute } = require('node-windows-audio-manager-switcher');

setDefaultPlaybackMute(true);  // Mute
setDefaultPlaybackMute(false); // Unmute
```

---

### 🔇 Mute / Unmute Specific Device by ID

```js
const { listDevices, muteDeviceById } = require('node-windows-audio-manager-switcher');

const target = listDevices()[0]; // Example: first device
muteDeviceById(target.id, true);  // Mute
muteDeviceById(target.id, false); // Unmute
```

---

## 📘 API Reference

| Function | Description |
|----------|-------------|
| `listDevices()` → `{ name, id, isDefault }[]` | Lists all active output devices |
| `setDefaultDevice(deviceId)` → `boolean` | Sets the default playback device |
| `setDefaultPlaybackMute(mute)` → `boolean` | Mute/unmute the default device |
| `muteDeviceById(deviceId, mute)` → `boolean` | Mute/unmute a specific device |

---

## 📂 Project Structure

```bash
node-windows-audio-manager-switcher/
├── index.js               # JS bindings to native addon
├── native/                # C++ source code (AudioSwitcher, DeviceUtils)
├── prebuilds/             # Precompiled binaries (.tar.gz)
├── build/                 # Generated at install (addon.node)
├── test/                  # Interactive example scripts
└── binding.gyp            # node-gyp config file
```

---

## 🧪 Dev Commands

```bash
# Build from source (dev)
npm run dev:build

# Generate prebuilt binary (for npm publish)
npm run dev:prebuild

# Run example tests
npm run dev:test:devices
npm run dev:test:set-default
npm run dev:test:mute-default
npm run dev:test:unmute-default
npm run dev:test:mute-device
npm run dev:test:unmute-device
```

---

## Prebuilt Native Addon

No toolchain? No problem.

We use [`prebuild`](https://github.com/prebuild/prebuild) to compile and package `.node` binaries:

```bash
npx prebuild --backend=node-gyp -t 20.13.1 --strip --napi
```

At install, `prebuild-install` downloads the correct binary from the `prebuilds/` folder.

> 📦 Works seamlessly for Windows x64 and Node.js 20.x+

---

## ⚠️ Troubleshooting

| Problem                                       | Solution                                                                                   |
|----------------------------------------------|--------------------------------------------------------------------------------------------|
| `node_api.h` not found                       | Make sure `node-addon-api` is installed: `npm install node-addon-api`                     |
| `addon.node` missing                         | Run `npm rebuild` or `npm run dev:build`                                                  |
| `CoCreateInstance` fails                     | Run your script with **Administrator privileges** (Right click > "Run as Administrator")  |
| `prebuild` fails                             | Ensure Node.js version matches and all required build tools are installed                 |
| ❌ `WindowsTargetPlatformVersion` error       | See **"Missing Windows SDK version" fix below**                                           |

---

### 🛠️ Missing Windows SDK Version

You might see an error like:

```bash
The Windows SDK version 10.0.22621.0 was not found. 
Install the required version of Windows SDK or change the SDK version...
```

This happens when your system **doesn't have the specific Windows SDK version** your build setup is looking for.

#### ✅ Solution 1: Let it use the latest installed SDK (Recommended)

Update your `binding.gyp` file and **remove the hardcoded SDK version**:

**Before (❌):**

```json
"msvs_windows_target_platform_version": "10.0.22621.0"
```

**After (✅):**
Just delete that line entirely.

Then rebuild everything:

```bash
npx node-gyp clean
npx node-gyp configure
npx node-gyp build
```

Node-gyp will now use **the latest installed SDK version** automatically.

---

#### ✅ Solution 2: Download the required SDK version

If you **want to use a specific SDK version** mentioned in the error (like `10.0.22621.0`), do this:

1. Visit the official Windows SDK archive:  
   👉 [https://developer.microsoft.com/en-us/windows/downloads/sdk-archive/](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive/)

2. Download and install the **exact SDK version** shown in the error.

3. Once installed, re-run:

   ```bash
   npm install
   ```

---

### 🔍 How to check installed SDKs

Run this in PowerShell:

```powershell
Get-ChildItem 'C:\Program Files (x86)\Windows Kits\10\Include' | Select-Object Name
```

You’ll see something like:

```bash
Name
----
10.0.19041.0
10.0.26100.0
```

Use one of these versions if you want to manually set it.

---

### 📌 Note on Prebuilt Binaries

This package ships with **prebuilt native binaries** for seamless installation — no C++ compiler, Visual Studio, or Windows SDK required!

✅ Just run:

```bash
npm install node-windows-audio-manager-switcher-switcher
```

> Under the hood, we use [`prebuild`](https://github.com/prebuild/prebuild) + [`prebuild-install`](https://github.com/prebuild/prebuild-install) to provide platform-specific `.node` binaries.

#### ℹ️ How It Works

- During `npm install`, `prebuild-install` checks for a compatible `.tar.gz` binary (based on your Node.js version and OS).
- If a matching binary is found (e.g. Node.js 20.x, Windows x64), it downloads and installs automatically.
- If **no matching binary** is available, it falls back to building from source — which requires **Visual Studio + Windows SDK**.

Here’s the updated section with a clean **batch-style** note and a list of supported versions so far:

---

#### 💡 Supported Node Versions

We currently provide prebuilt native binaries for the following Node.js versions:

| Node.js Version | ABI Version | Status     |
|-----------------|-------------|------------|
| ✅ 22.x          | `v127`      | Supported  |
| ✅ 21.x          | `v130`      | Supported  |
| ✅ 20.x          | `v115`      | Supported  |
| ✅ 19.x          | `v111`      | Supported  |
| ✅ 18.x (LTS)    | `v108`      | Supported  |

> 💡 **Note:** If you're using a different version of Node.js and encounter the message  
> `prebuild-install warn install No prebuilt binaries found`, you can either:
>
> 1. **Switch to a supported version** (recommended), or  
> 2. **Manually build from source** by running:

```bash
npm install --build-from-source
```

- _(More coming soon...)_

---

If you're using a newer Node.js version and see an install error like:

```bash
prebuild-install warn install No prebuilt binaries found (target=22.14.0 ...)
```

This means we haven't published a prebuilt binary for your Node version **yet**. You can either:

- ⚙️ Build from source (requires build tools), or
- 🧩 Downgrade to a supported Node.js version (e.g. Node 20.x)

---

## 🙌 Contributing

We welcome PRs and ideas! You can contribute by:

- 🎚️ Adding volume control APIs
- 🔁 Listening to real-time device change events
- ✅ Writing unit tests (e.g. with `jest` or `mocha`)

---

## ❤️ Credits

- Microsoft Core Audio APIs (`IMMDevice`, `IPolicyConfig`, `IAudioEndpointVolume`)
- [node-addon-api](https://github.com/nodejs/node-addon-api) (N-API C++ bindings)
- Inspiration: [tartakynov/audioswitch](https://github.com/tartakynov/audioswitch)
