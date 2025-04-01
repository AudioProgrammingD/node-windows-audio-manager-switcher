const { setDefaultPlaybackMute } = require('../index');

console.log('[JS] Toggling mute for default playback device...');

// Example: Mute the default device
const success = setDefaultPlaybackMute(true);

console.log(success ? '[✓] Default device muted.' : '[x] Failed to mute default device.');
