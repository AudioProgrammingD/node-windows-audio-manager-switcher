const readline = require('readline');
const { listDevices, setDefaultDevice } = require('../index');

// Step 1: List devices
const devices = listDevices();
if (!devices.length) {
    console.log('❌ No playback devices found.');
    process.exit(1);
}

console.log('\n🎧 Available Playback Devices:\n');
devices.forEach((device, index) => {
    console.log(`${index + 1}. ${device.name} [${device.id}]`);
});

// Step 2: Ask user for a selection
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
});

rl.question('\n🔧 Enter the number of the device to set as default: ', (input) => {
    const index = parseInt(input) - 1;

    if (isNaN(index) || index < 0 || index >= devices.length) {
        console.log('❌ Invalid choice.');
        rl.close();
        return;
    }

    const chosen = devices[index];
    const success = setDefaultDevice(chosen.id);

    if (success) {
        console.log(`✅ Successfully set "${chosen.name}" as default output device.`);
    } else {
        console.log(`❌ Failed to set default device.`);
    }

    rl.close();
});
