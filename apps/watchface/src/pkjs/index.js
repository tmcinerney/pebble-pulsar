var Clay = require('pebble-clay');
var clayConfig = require('./config.json');

// Display-mode values, matching enum DisplayMode in pulsar.c.
var MODE_STEPS = '3';
var MODE_HEART_RATE = '5';
// Dot Meter values, matching enum BeadMode.
var BEAD_STEPS = '0';

// AIDEV-NOTE: aplite is the only platform with no health service at all, so it has neither steps nor
// heart rate. Every other platform has steps. An HR *sensor* is a separate question the platform cannot
// answer -- basalt has health but no sensor -- and the settings page has no way to ask the watch, so it
// is treated as present wherever health is and left to the watch to hide. pulsar.c gates the display
// itself on health_service_metric_accessible(), which is authoritative.
var NO_HEALTH = ['aplite'];

function platform() {
  try {
    var info = Pebble.getActiveWatchInfo && Pebble.getActiveWatchInfo();
    return (info && info.platform) || null;
  } catch (e) {
    return null;
  }
}

function dropOptions(item, values) {
  if (!item || !item.config || !item.config.options) { return; }
  item.config.options = item.config.options.filter(function(o) {
    return values.indexOf(String(o.value)) === -1;
  });
}

// AIDEV-NOTE: Clay caches settings on the PHONE in localStorage['clay-settings'], separately from the
// watch's own persist store. Clearing the cache and rebuilding the form is what makes a reset stick --
// otherwise the next Save would re-send the stale cached values straight back to the watch. The wearer
// still has to press Save, which is what actually pushes the defaults across.
var clay = new Clay(clayConfig, function() {
  var cfg = this;

  cfg.on(cfg.EVENTS.AFTER_BUILD, function() {
    var reset = cfg.getItemById('resetDefaults');
    if (reset) {
      reset.on('click', function() {
        localStorage.removeItem('clay-settings');
        location.reload();
      });
    }

    // Offer only what this watch can actually measure.
    var p = platform();
    if (p && NO_HEALTH.indexOf(p) !== -1) {
      ['AppKeyCycleSlot1', 'AppKeyCycleSlot2', 'AppKeyCycleSlot3',
       'AppKeyCycleSlot4', 'AppKeyCycleSlot5'].forEach(function(key) {
        dropOptions(cfg.getItemByMessageKey(key), [MODE_STEPS, MODE_HEART_RATE]);
      });
      dropOptions(cfg.getItemByMessageKey('AppKeyBeadMode'), [BEAD_STEPS]);

      // Step Goal and its alert have nothing to act on without a pedometer.
      ['AppKeyStepGoal', 'AppKeyStepCelebration'].forEach(function(key) {
        var item = cfg.getItemByMessageKey(key);
        if (item && item.hide) { item.hide(); }
      });
    }
  });
});
