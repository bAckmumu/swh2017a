/* MidiMassOrgel
  by Manuel Hartmann <https://github.com/bAckmumu/swh2017a>
*/

#include <Servo.h>

int servoPin = 6;     // fisrt pin for the servos

int servoCount = 8;

Servo pipeServos[8];  // create an array of servo objects


void setup() {
  // init servos
  for (int i = 0; i < servoCount; i++)
  {
    pipeServos[i].attach(i + servoPin);
  }

  servosTest();
}

void loop() {
  //  myservo.write(0);

  
}

/*
  test function to check if all servos are working, called in setup()
*/
void servosTest() {
  // 1. close pipes
  for (int s = 0; s < servoCount; s++)
  {
    closePipe(s);
  }

  // 2. open pipes one by one
  for (int s = 0; s < servoCount; s++)
  {
    openPipe(s);
    delay(100);
  }
  delay(500);

  // 3. pipes closed one by one
  for (int s = 0; s < servoCount; s++)
  {
    closePipe(s);
    delay(100);
  }
}

/*
  helper function to open pipe
*/
void openPipe(int servoNumber) {
  pipeServos[servoNumber].write(90);
}

/*
  helper function to close pipe
*/
void closePipe(int servoNumber) {
  pipeServos[servoNumber].write(0);
}


