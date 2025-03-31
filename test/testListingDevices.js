const { addon } = require('../index');

const devices = addon.listDevices();
console.log('Available Playback Devices:');
devices.forEach((device, index) => {
    console.log(`${index + 1}. ${device.name} [${device.id}]`);
});
