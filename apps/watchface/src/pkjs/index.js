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

function getPlatform() {
  try {
    var info = Pebble.getActiveWatchInfo && Pebble.getActiveWatchInfo();
    return (info && info.platform) || null;
  } catch (e) {
    return null;
  }
}

function dropOptions(config, messageKey, dropValues) {
  for (var i = 0; i < config.length; i++) {
    var entry = config[i];
    if (entry.items) {
      dropOptions(entry.items, messageKey, dropValues);
    } else if (entry.messageKey === messageKey && entry.options) {
      entry.options = entry.options.filter(function(o) {
        return dropValues.indexOf(String(o.value)) === -1;
      });
    }
  }
}

function removeItems(config, messageKeys) {
  for (var i = config.length - 1; i >= 0; i--) {
    var entry = config[i];
    if (entry.items) {
      removeItems(entry.items, messageKeys);
    } else if (entry.messageKey && messageKeys.indexOf(entry.messageKey) !== -1) {
      config.splice(i, 1);
    }
  }
}

function filterHealthConfig(config) {
  var p = getPlatform();
  if (p && NO_HEALTH.indexOf(p) !== -1) {
    ['AppKeyCycleSlot1', 'AppKeyCycleSlot2', 'AppKeyCycleSlot3',
     'AppKeyCycleSlot4', 'AppKeyCycleSlot5'].forEach(function(key) {
      dropOptions(config, key, [MODE_STEPS, MODE_HEART_RATE]);
    });
    dropOptions(config, 'AppKeyBeadMode', [BEAD_STEPS]);
    removeItems(config, ['AppKeyStepGoal', 'AppKeyStepCelebration']);
  }
}

// Filter clayConfig in PKJS context before initializing Clay
filterHealthConfig(clayConfig);

var clay = new Clay(clayConfig, function() {
  var cfg = this;

  cfg.on(cfg.EVENTS.AFTER_BUILD, function() {
    var reset = cfg.getItemById('resetDefaults');
    if (!reset) { return; }
    reset.on('click', function() {
      cfg.getAllItems().forEach(function(item) {
        if (item && item.config && item.config.defaultValue !== undefined && typeof item.set === 'function') {
          item.set(item.config.defaultValue);
        }
      });
    });
  });
});
