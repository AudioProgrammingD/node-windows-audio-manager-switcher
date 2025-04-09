/**
 * @file index.js
 * @module node-windows-audio-manager-switcher
 * @description Primary interface for the Windows Audio Manager native addon.
 *              Provides JavaScript bindings to native C++ audio device management functions
 *              through Node.js N-API. This module enables:
 *              - Device enumeration
 *              - Default device configuration
 *              - Mute control for both default and specific devices
 * 
 * @author [sameerbk201]
 * @copyright [2025] [sameerbk201]
 */
const addon = require('./build/Release/addon.node');
/**
 * Retrieves all available audio playback devices on the system.
 * @function listDevices
 * @returns {Array<DeviceInfo>} Array of device information objects
 * @property {string} name - Human-readable device name
 * @property {string} id - System-unique device identifier
 * @property {boolean} isDefault - True if device is current default playback device
 * 
 * @example
 * const { listDevices } = require('node-windows-audio-manager-switcher');
 * const devices = listDevices();
 * console.log('Available playback devices:');
 * devices.forEach(device => {
 *   const status = device.isDefault ? '(Default)' : '';
 *   console.log(`- ${device.name} ${status}`);
 * });
 */

/**
 * Changes the default audio playback device.
 * @function setDefaultDevice
 * @param {string} deviceId - The ID of the device to set as default (from listDevices)
 * @returns {boolean} True if operation succeeded, false otherwise
 * 
 * @example
 * const { listDevices, setDefaultDevice } = require('node-windows-audio-manager-switcher');
 * const [firstDevice] = listDevices();
 * if (firstDevice) {
 *   const success = setDefaultDevice(firstDevice.id);
 *   console.log(`Default device change ${success ? 'succeeded' : 'failed'}`);
 * }
 */

/**
 * Mutes or unmutes the default audio playback device.
 * @function setDefaultPlaybackMute
 * @param {boolean} muteState - True to mute, false to unmute
 * @returns {boolean} True if operation succeeded, false otherwise
 * 
 * @example
 * const { setDefaultPlaybackMute } = require('node-windows-audio-manager-switcher');
 * // Mute the default device
 * setDefaultPlaybackMute(true);
 * 
 * // Unmute after 3 seconds
 * setTimeout(() => setDefaultPlaybackMute(false), 3000);
 */

/**
 * Mutes or unmutes a specific audio device by its ID.
 * @function muteDeviceById
 * @param {string} deviceId - The target device ID (from listDevices)
 * @param {boolean} muteState - True to mute, false to unmute
 * @returns {boolean} True if operation succeeded, false otherwise
 * 
 * @example
 * const { listDevices, muteDeviceById } = require('node-windows-audio-manager-switcher');
 * const [speakers, headphones] = listDevices();
 * // Mute speakers if available
 * if (speakers) {
 *   muteDeviceById(speakers.id, true);
 * }
 */
module.exports = {
    addon,
    listDevices: addon.listDevices,
    setDefaultDevice: addon.setDefaultDevice,
    setDefaultPlaybackMute: addon.setDefaultPlaybackMute,
    muteDeviceById: addon.muteDeviceById
};
