const readline = require('readline');
const { listDevices, muteDeviceById } = require('../index');

// Step 1: List devices
const devices = listDevices();
if (!devices.length) {
    console.log('❌ No playback devices found.');
    process.exit(1);
}

console.log('\n🔊 Available Playback Devices:\n');
devices.forEach((device, index) => {
    console.log(`${index + 1}. ${device.name} [${device.id}]`);
});

// Step 2: Ask user to pick a device to unmute
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
});

rl.question('\n🔧 Enter the number of the device to unmute: ', (input) => {
    const index = parseInt(input) - 1;

    if (isNaN(index) || index < 0 || index >= devices.length) {
        console.log('❌ Invalid choice.');
        rl.close();
        return;
    }

    const chosen = devices[index];
    console.log(`\n🔊 Attempting to unmute "${chosen.name}"...`);

    const success = muteDeviceById(chosen.id, false);

    if (success) {
        console.log(`✅ "${chosen.name}" has been unmuted.`);
    } else {
        console.log(`❌ Failed to unmute "${chosen.name}".`);
    }

    rl.close();
});
