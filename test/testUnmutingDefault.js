const { setDefaultPlaybackMute } = require('../index');

console.log('[JS] Toggling mute for default playback device...');

// Example: Mute the default device
const success = setDefaultPlaybackMute(false);

console.log(success ? '[✓] Default device unmuted.' : '[x] Failed to unmute default device.');
