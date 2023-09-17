#ifndef MyChords_h
#define MyChords_h
#include <Arduino.h>

struct Chord {
  int root;
  int note1;
  int note2;
  String _name;
};

class Progression {
public:
  Progression(struct Chord chord1, struct Chord chord2, struct Chord chord3, struct Chord chord4, int chan, String scale, String description);
  int getChordRoot(int i);
  int getChordChan();
  Chord getChords(int i);
  String getScale();
  String getDescription();

private:
  struct Chord midi_chords[4];
  int midi_chan;
  String _scale;
  String _description;
};

#endif
