/* MidiMassOrgel
  by Manuel Hartmann <https://github.com/bAckmumu/swh2017a>
*/

#include <Servo.h>
#include <SoftwareSerial.h>

/*
  MIDI
*/
#define MIDI_OUT_PIN 2  // MIDI OUT is connected to Arduino pin ...
#define MIDI_IN_PIN 3   // MIDI IN is connected to Arduino pin ...,

SoftwareSerial midiSerial(MIDI_IN_PIN, MIDI_OUT_PIN);

const char* NOTE_NAMES[12] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
#define MIDI_CMD_NOTE_ON 0x90
#define MIDI_CMD_NOTE_OFF 0x80
#define MIDI_CMD_SENSE 0xf0


/*
  Servos
*/
#define SERVO_START_PIN 6 // first pin for the servos
#define SERVO_COUNT 8     // number of servos equates number of notes

Servo pipeServos[8];      // array of servo objects

// with 8 pipes the organ can not play all notes
// this setting is C-Dur, only whole steps
// 42 will be ingnored by the servo functions
const int NOTE_PIPES[12] = { 0, 42, 1, 42, 2, 3, 42, 4, 42, 5, 42, 6 };


/*
  Functions to handle the MIDI
*/
void midi_init() {
  midiSerial.begin(31250);
}

void midiTest() {
  // Play some notes
  for (int i = 0; i < 12; i++) {
    int n = 12 * 4 + i;
    Serial.print("Playing "); Serial.println(n);
    midi_noteOn(0, n, 100);
    on_midi_noteOn(0, n, 100);
    delay(400);
    midi_noteOff(0, n, 0);
    on_midi_noteOff(0, n, 0);
  }
}

int midi_read() {
  // Read and return

  return midiSerial.read();
}

void midi_write(int d) {
  midiSerial.write(d);
}

// This function sends a "Note ON" out of the MIDI OUT port
void midi_noteOn(uint8_t midi_chn, uint8_t midi_note, uint8_t midi_velocity) {
  midi_write(MIDI_CMD_NOTE_ON + midi_chn);
  midi_write(midi_note);
  midi_write(midi_velocity);
}

// This function sends a "Note OFF" out of the MIDI OUT port
void midi_noteOff(uint8_t midi_chn, uint8_t midi_note, uint8_t midi_velocity) {
  midi_write(MIDI_CMD_NOTE_OFF + midi_chn);
  midi_write(midi_note);
  midi_write(midi_velocity);
}


// This function must be called continuously to check for incoming data on the MIDI IN port
void midi_update() {
  uint8_t midi_c, midi_cmd, midi_chn, midi_note, midi_velocity;

  if (midiSerial.available()) {
    midi_c = midiSerial.read();
    Serial.print("RAW "); Serial.println(midi_c, HEX);

    //if (midi_c == 0) return;
    midi_cmd = midi_c & 0xf0;
    midi_chn = midi_c & 0x0f;

    //Serial.print(" cmd="); Serial.println(midi_cmd, HEX);

    switch (midi_cmd) {

      case MIDI_CMD_NOTE_ON:  // Note on
        midi_note = midi_read();
        midi_velocity = midi_read();
        on_midi_noteOn(midi_chn, midi_note, midi_velocity); // Call the callback
        break;

      case MIDI_CMD_NOTE_OFF:  // Note off
        midi_note = midi_read();
        midi_velocity = midi_read();
        on_midi_noteOff(midi_chn, midi_note, midi_velocity); // Call the callback
        break;

      default:
        // Unknown command. Just ignore!
        midi_note = 0;
        midi_velocity = 0;
    }
  }
}


// This function is called whenever a note is pressed on the MIDI IN port
void on_midi_noteOn(uint8_t midi_chn, uint8_t midi_note, uint8_t midi_velocity) {


  // Play note!
  int note = midi_note % 12;
  int octave = midi_note / 12;
  openPipe(NOTE_PIPES[note]);


  Serial.print("Note ON: ");
  Serial.println(midi_note);

  //Serial.print(NOTE_NAMES[note]);
  //Serial.print(octave);
  //Serial.print("  volume=");
  //Serial.println(midi_velocity);

  // Falls jemand wissen will, was fuer eine FREQUENZ die aktuelle Note hat:
  //float frq = 220.0 * pow(2.0, ((float)midi_note / 12.0));
  //Serial.println(frq);


  // MIDI THRU: Send the note thru to MIDI OUT
  midi_noteOn(midi_chn, midi_note, midi_velocity);

}

// This function is called whenever a note is released on the MIDI IN port
void on_midi_noteOff(uint8_t midi_chn, uint8_t midi_note, uint8_t midi_velocity) {


  // Stop note!
  int note = midi_note % 12;
  int octave = midi_note / 12;
  closePipe(NOTE_PIPES[note]);


  Serial.print("Note OFF: ");
  Serial.println(midi_note);
  //Serial.print(NOTE_NAMES[midi_note % 12]);
  //Serial.println(midi_note / 12);


  // MIDI THRU: Send the noteOff thru to MIDI OUT
  //midi_noteOff(midi_chn, midi_note, midi_velocity);

}


/*
   Functions to handle the servos
*/
void servosInit() {
  for (int i = 0; i < SERVO_COUNT; i++)
  {
    pipeServos[i].attach(i + SERVO_START_PIN);
  }
}

// test function to check if all servos are working, called in setup()
void servosTest() {
  // 1. close pipes
  for (int s = 0; s < SERVO_COUNT; s++)
  {
    closePipe(s);
  }

  // 2. open pipes one by one
  for (int s = 0; s < SERVO_COUNT; s++)
  {
    openPipe(s);
    delay(100);
  }
  delay(500);

  // 3. pipes closed one by one
  for (int s = 0; s < SERVO_COUNT; s++)
  {
    closePipe(s);
    delay(100);
  }
  delay(500);
}

// helper function to open pipe
void openPipe(int servoNumber) {
  Serial.print("Opening pipe "); Serial.println(servoNumber);
  if (servoNumber < SERVO_COUNT) {
    pipeServos[servoNumber].write(90);
  }
}

// helper function to close pipe
void closePipe(int servoNumber) {
  Serial.print("Closing pipe "); Serial.println(servoNumber);
  if (servoNumber < SERVO_COUNT) {
    pipeServos[servoNumber].write(0);
  }
}


/*
   Arduino setup() and loop()
*/
void setup() {

  midi_init();  // Start MIDI

  servosInit();

  Serial.begin(9600);

  servosTest();

  midiTest();
}

void loop() {

  midi_update();  // Update MIDI

}

