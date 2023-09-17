#include "MyChords.h"

Progression::Progression(Chord chord1, Chord chord2, Chord chord3, Chord chord4, int chan, String scale, String description) {
  midi_chords[0] = chord1;
  midi_chords[1] = chord2;
  midi_chords[2] = chord3;
  midi_chords[3] = chord4;
  midi_chan = chan;
  _scale = scale;
  _description = description;
}

int Progression::getChordRoot(int i) {
  return midi_chords[i].root;
}

int Progression::getChordChan() {
  return midi_chan;
}

Chord Progression::getChords(int i) {
  return midi_chords[i];
}

String Progression::getScale() {
  return _scale;
}

String Progression::getDescription() {
  return _description;
}
