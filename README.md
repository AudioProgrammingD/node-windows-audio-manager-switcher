# 🎧 Node Windows Audio Manager

> 🔧 A lightweight Node.js native addon for managing **Windows audio playback devices**.  
> Seamlessly integrated with Windows Core Audio APIs using **C++ + N-API**, prebuilt for plug-and-play usage.

---

## 🚀 Features

- 🎚️ **List all active audio output devices**
- 🔄 **Set a device as default playback** (System, Multimedia, Communication roles)
- 🔇 **Mute/unmute** devices (coming soon)
- 🧩 Native performance via C++ Core Audio integration
- 💡 Prebuilt binaries for plug-and-play (no build tools needed!)

---

## 📦 Installation

```bash
npm i node-windows-audio-manager
```

✅ Automatically installs native bindings (prebuilt binaries for Windows)

> 💡 No need to compile manually unless you're modifying C++ sources.

---

## 📁 Folder Structure (Internals)

```bash
node-windows-audio-manager/
├── index.js               # Entry point for users
├── native/                # C++ code (addon.cpp + src)
├── prebuilds/             # Prebuilt binaries (plug-and-play)
├── build/                 # Created automatically after install (addon.node)
├── test/                  # Example usage
└── binding.gyp            # Node-gyp config
```

---

## 🧪 Example Usage

### 🔍 List Devices

```js
const { listDevices } = require('node-windows-audio-manager');

const devices = listDevices();
devices.forEach((device, index) => {
  console.log(`${index + 1}. ${device.name} [${device.id}]`);
});
```

---

### 🎚️ Set Default Playback Device

```js
const { setDefaultPlayback } = require('node-windows-audio-manager');

const devices = listDevices();
const targetDevice = devices.find(d => d.name.includes('Speakers'));

if (targetDevice) {
  const success = setDefaultPlayback(targetDevice.id);
  console.log(success ? '✅ Switched default device.' : '❌ Failed to switch.');
} else {
  console.error('Device not found!');
}
```

---

## 📘 API Reference

### `listDevices(): Array<{ name: string, id: string }>`

Returns a list of currently active audio playback devices.

---

### `setDefaultPlayback(deviceId: string): boolean`

Sets the specified device ID as the default system playback device for:

- Console
- Multimedia
- Communications

Returns:

- `true` if successful
- `false` on failure (invalid ID, permissions, etc.)

---

## 💻 Prebuilt Native Addon

No native build tools needed on install:

- Binaries are automatically downloaded (via `prebuild-install`)
- Targets Windows x64 / Node.js 20.x

> 💡 If you’re building from source:

```bash
npm run build       # Clean + configure + build native
npm run prebuild    # Create prebuilt .tar.gz (for npm publish)
```

---

## 🛠️ Scripts

```json
"scripts": {
  "install": "prebuild-install || node-gyp rebuild",
  "build": "node-gyp clean configure build",
  "prebuild": "prebuild --backend=node-gyp -t 20.13.1 --strip --napi",
  "test:devices": "node ./test/testListingDevices.js",
  "test:setdefault": "node ./test/testSettingDefaultPlayback.js"
}
```

---

## 🐞 Troubleshooting

- **❌ `node_api.h` not found**  
  Ensure `node-addon-api` is installed, and use `"<!(node -p \"require('node-addon-api').include\")"` in your `binding.gyp`.

- **❌ Prebuild doesn't work**  
  Rebuild manually:

  ```bash
  npm install --build-from-source
  ```

- **❌ Device switching fails silently**  
  Run your app with **admin privileges** to ensure access to device policies.

---

## 🧑‍💻 Contributing

Feel free to open issues or submit PRs. This project can be extended to support:

- 🔇 Mute/unmute devices
- 📈 Get/set volume levels
- 🔁 Monitor device changes (hotplug events)

---

## ❤️ Credits

- Built with [Node-API (N-API)](https://nodejs.org/api/n-api.html)
- Windows COM APIs (Core Audio)
- Based on [`IPolicyConfig`](https://github.com/tartakynov/audioswitch/blob/master/IPolicyConfig.h) for default switching (undocumented)
