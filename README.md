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
npm install node-windows-audio-manager
```

✅ Works out of the box on Windows  
✅ No Visual Studio or compilation needed

> 💬 If you modify native C++ code, see [Build From Source](#prebuilt-native-addon)

---

## 🧠 Usage Examples

### 🔍 List Playback Devices

```js
const { listDevices } = require('node-windows-audio-manager');

const devices = listDevices();
devices.forEach((device, index) => {
  console.log(`${index + 1}. ${device.name} [${device.id}] Default: ${device.isDefault}`);
});
```

---

### 🎚️ Set Default Playback Device

```js
const { listDevices, setDefaultPlayback } = require('node-windows-audio-manager');

const devices = listDevices();
const target = devices.find(d => d.name.includes("Speakers"));

if (target) {
  const success = setDefaultPlayback(target.id);
  console.log(success ? "✅ Set as default!" : "❌ Failed to set device.");
}
```

---

### 🔇 Mute / Unmute Default Device

```js
const { setDefaultPlaybackMute } = require('node-windows-audio-manager');

setDefaultPlaybackMute(true);  // Mute
setDefaultPlaybackMute(false); // Unmute
```

---

### 🔇 Mute / Unmute Specific Device by ID

```js
const { listDevices, muteDeviceById } = require('node-windows-audio-manager');

const target = listDevices()[0]; // Example: first device
muteDeviceById(target.id, true);  // Mute
muteDeviceById(target.id, false); // Unmute
```

---

## 📘 API Reference

| Function | Description |
|----------|-------------|
| `listDevices()` → `{ name, id, isDefault }[]` | Lists all active output devices |
| `setDefaultPlayback(deviceId)` → `boolean` | Sets the default playback device |
| `setDefaultPlaybackMute(mute)` → `boolean` | Mute/unmute the default device |
| `muteDeviceById(deviceId, mute)` → `boolean` | Mute/unmute a specific device |

---

## 📂 Project Structure

```bash
node-windows-audio-manager/
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

| Problem                            | Solution                                                              |
|-----------------------------------|-----------------------------------------------------------------------|
| `node_api.h` not found            | Make sure `node-addon-api` is installed                              |
| `addon.node` missing              | Run `npm rebuild` or `npm run dev:build`                             |
| `CoCreateInstance` fails          | Run your script with **Administrator privileges**                    |
| `prebuild` fails                  | Ensure Node.js version matches and all build tools are set up        |

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
