#include <Bounce.h>
#include "MyChords.h"

/*************** Define chords that we will be using ********************/
/* Restrict each chord to contain 1 root note and exactly 2 other notes */
const struct Chord C3 = {60, 64, 67, "C3"};
const struct Chord G3 = {67, 71, 74, "G3"};
const struct Chord Am3 = {69, 72, 76, "Am3"};
const struct Chord F3 = {65, 69, 72, "F3"};
const struct Chord Cm3 = {60, 55, 63, "Cm3"};
const struct Chord Bb2 = {58, 53, 62, "Bb2"};
const struct Chord Ab3 = {68, 63, 72, "Ab3"};
const struct Chord A_IV = {57, 53, 64, "A_IV"};
const struct Chord B_IV = {59, 55, 64, "B_IV"};
const struct Chord C_IV = {60, 57, 64, "C_IV"};
const struct Chord D_IV = {62, 59, 64, "D_IV"};

/*  Note mappings  */

/************************************************************************/

const int NUM_CHORD_BUTTONS = 4;
const int NUM_CHORD_PROGRESSIONS = 3;
const int NUM_NOTE_LEDS = 12;

const int DEBOUNCE_TIME = 5;

// Initiate the chords
int playing_chordset = 0;  // Chords[playing_chordset]
int playing_note = -1;  // Chords[i][playing_note]
int MIDI_CHORD_VEL = 80;
Progression Progressions[NUM_CHORD_PROGRESSIONS] = {
  Progression (C3, G3, Am3, F3, 3, "C Major", "Delightful"),    // C Major
  Progression (Cm3, Bb2, Ab3, G3, 4, "C Minor", "Melancholy"),    // C Minor
  Progression (A_IV, B_IV, C_IV, D_IV, 5, "II, IV, V, VII Chord with E", "Conflict to resolved")     // Interstellar
};

// Button pullup pins 18-21
const int BUTTON_START_PIN = 18;
Bounce chordButtons[NUM_CHORD_BUTTONS] = 
{
  Bounce (BUTTON_START_PIN + 0, DEBOUNCE_TIME),
  Bounce (BUTTON_START_PIN + 1, DEBOUNCE_TIME),
  Bounce (BUTTON_START_PIN + 2, DEBOUNCE_TIME),
  Bounce (BUTTON_START_PIN + 3, DEBOUNCE_TIME)
};

// Potentiometer analog pin 14 (A0)
const int POTENTIO = 14;

// LED lights pins 0-11
const int LED_START_PIN = 0;

void setup() {
  for (int i = 0; i < NUM_CHORD_BUTTONS; i++) {
    pinMode(BUTTON_START_PIN + i, INPUT_PULLUP);
  }
  for (int i = 0; i < NUM_NOTE_LEDS; i++) {
    pinMode(LED_START_PIN + i, OUTPUT);
  }
  playing_chordset = updateChord(analogRead(A0));
  Serial.begin(9600);
}

void loop() {
  int potentio = analogRead(A0);
  int next_chordset = updateChord(potentio);
  int next_note = playing_note;
  bool keyPressed = false;
  
  for (int i = 0; i < NUM_CHORD_BUTTONS; i++) {
    chordButtons[i].update();
    if (chordButtons[i].fallingEdge()) {
      next_note = i;  // We only care about keyDown events in chord, since the chord will continue to play after button released
      keyPressed = true;
    }
  }

  // Action if a key is pressed or if the chord changes
  if (next_chordset != playing_chordset || keyPressed) {
    // First switch off the currently playing chord note
    usbMIDI.sendNoteOff(Progressions[playing_chordset].getChordRoot(playing_note), 0, Progressions[playing_chordset].getChordChan());
    switchLights(Progressions[playing_chordset].getChords(playing_note), "OFF");
    clearTerminal();
    // Send new chord note if chord changes or note changes
    if ((next_chordset != playing_chordset && playing_note != -1) || next_note != playing_note) {
      usbMIDI.sendNoteOn(Progressions[next_chordset].getChordRoot(next_note), MIDI_CHORD_VEL, Progressions[next_chordset].getChordChan());
      switchLights(Progressions[next_chordset].getChords(next_note), "ON");
      printPrompt(Progressions[next_chordset], Progressions[next_chordset].getChords(next_note));
      playing_chordset = next_chordset;
      playing_note = next_note;
    } else {
      // Reset to no note playing if double pressed (stop chord)
      playing_note = -1;
    }
  }
  
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}

int updateChord(int potentio) {
  return map(potentio, 0, 1023, 0, NUM_CHORD_PROGRESSIONS-1);
}

int getLedPin(int midinote) {
  return (midinote % 12) + LED_START_PIN;
}

void switchLights(Chord chord, String mode) {
  int pin1 = getLedPin(chord.root);
  int pin2 = getLedPin(chord.note1);
  int pin3 = getLedPin(chord.note2);
  if (mode == "OFF") {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
  } else if (mode == "ON") {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
  }
}

void printPrompt(Progression progression, Chord chord) {
  Serial.print("Progression: ");
  Serial.println(progression.getScale());
  Serial.print("Possible chords that make up this progression: ");
  Serial.print(progression.getChords(0)._name);
  Serial.print(" | ");
  Serial.print(progression.getChords(1)._name);
  Serial.print(" | ");
  Serial.print(progression.getChords(2)._name);
  Serial.print(" | ");
  Serial.println(progression.getChords(3)._name);
  Serial.println(progression.getDescription());
  Serial.println();
  Serial.print("Chord currently playing: ");
  Serial.println(chord._name);
  Serial.print("It is made up of notes: ");
  Serial.print(getNoteName(chord.root));
  Serial.print(" ");
  Serial.print(getNoteName(chord.note1));
  Serial.print(" ");
  Serial.println(getNoteName(chord.note2));
}

void clearTerminal() {
  for (int i = 0; i < 30; i++) {
    Serial.println();
  }
}

String getNoteName(int midinote) {
  switch (midinote % 12) {
    case 0: 
      return "C";
    case 1:
      return "C#/Db";
    case 2:
      return "D";
    case 3:
      return "Eb";
    case 4:
      return "E";
    case 5:
      return "F";
    case 6:
      return "F#/Gb";
    case 7:
      return "G";
    case 8:
      return "Ab";
    case 9:
      return "A";
    case 10:
      return "Bb";
    case 11:
      return "B";
    default:
      return "";
  }
}
