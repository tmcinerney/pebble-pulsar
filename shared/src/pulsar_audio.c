#include "pulsar_audio.h"

void pulsar_audio_init(void) {
}

void pulsar_sound_start(bool audio, bool vibe) {
  if (vibe) {
    vibes_short_pulse();
  }
  if (audio) {
#if PBL_API_EXISTS(speaker_play_tone)
    speaker_play_tone(1760, 60, 60, SpeakerWaveformSquare);
#endif
  }
}

void pulsar_sound_stop(bool audio, bool vibe) {
  if (vibe) {
    vibes_short_pulse();
  }
  if (audio) {
#if PBL_API_EXISTS(speaker_play_tone)
    speaker_play_tone(880, 80, 60, SpeakerWaveformSquare);
#endif
  }
}

void pulsar_sound_lap(bool audio, bool vibe) {
  if (vibe) {
    vibes_double_pulse();
  }
  if (audio) {
#if PBL_API_EXISTS(speaker_play_tone)
    speaker_play_tone(2093, 40, 70, SpeakerWaveformSquare);
#endif
  }
}

void pulsar_sound_reset(bool audio, bool vibe) {
  if (vibe) {
    vibes_short_pulse();
  }
  if (audio) {
#if PBL_API_EXISTS(speaker_play_tone)
    speaker_play_tone(587, 80, 50, SpeakerWaveformSawtooth);
#endif
  }
}

void pulsar_sound_countdown_tick(int sec_remaining, bool audio, bool vibe) {
  if (sec_remaining > 0 && sec_remaining <= 3) {
    if (vibe) {
      vibes_short_pulse();
    }
    if (audio) {
#if PBL_API_EXISTS(speaker_play_tone)
      speaker_play_tone(1046 + ((4 - sec_remaining) * 200), 50, 60, SpeakerWaveformSquare);
#endif
    }
  } else if (sec_remaining == 0) {
    if (vibe) {
      vibes_double_pulse();
    }
    if (audio) {
#if PBL_API_EXISTS(speaker_play_tone)
      speaker_play_tone(2093, 200, 80, SpeakerWaveformSquare);
#endif
    }
  }
}

void pulsar_sound_alarm_pulse(bool audio, bool vibe) {
  if (vibe) {
    vibes_double_pulse();
  }
  if (audio) {
#if PBL_API_EXISTS(speaker_play_tone)
    speaker_play_tone(2093, 120, 80, SpeakerWaveformSquare);
#endif
  }
}
