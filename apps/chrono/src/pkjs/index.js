var Clay = require('pebble-clay');
var clayConfig = require('./config.json');

// AIDEV-NOTE: Clay caches settings on the PHONE in localStorage['clay-settings'], separately from the
// watch's own persist store. Clearing the cache and rebuilding the form is what makes a reset stick --
// otherwise the next Save would re-send the stale cached values straight back to the watch. The wearer
// still has to press Save, which is what actually pushes the defaults across.
var clay = new Clay(clayConfig, function() {
  var cfg = this;
  cfg.on(cfg.EVENTS.AFTER_BUILD, function() {
    var reset = cfg.getItemById('resetDefaults');
    if (!reset) { return; }
    reset.on('click', function() {
      localStorage.removeItem('clay-settings');
      location.reload();
    });
  });
});
