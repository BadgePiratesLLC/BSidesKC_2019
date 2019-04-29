/*
  Melody

 Plays a melody

 circuit:
 * 8-ohm speaker on digital pin 8

 created 21 Jan 2010
 modified 30 Aug 2011
 by Tom Igoe

This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Tone

 */

// notes in the melody:
int melody[] = {
  NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4,NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_FS4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  2, 2, 4, 4, 4, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4, 8, 8, 4, 4, 2
};
