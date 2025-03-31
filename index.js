/**
 * @file index.js
 * @description Entry point for the `node-windows-audio-manager` native addon.
 *              Exports native C++ bindings for Windows audio device management.
 */

const addon = require('./build/Release/addon.node');

/**
 * Lists all available audio playback devices on the system.
 *
 * @function listDevices
 * @returns {Array<{ name: string, id: string }>} An array of device objects with `name` and `id`.
 * 
 * @example
 * const { listDevices } = require('node-windows-audio-manager');
 * const devices = listDevices();
 * devices.forEach((d, i) => {
 *   console.log(`${i + 1}. ${d.name} [${d.id}]`);
 * });
 */
module.exports = {
    addon,
    listDevices: addon.listDevices,
    setDefaultDevice: addon.setDefaultDevice
};
