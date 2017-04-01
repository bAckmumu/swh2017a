/* MidiMassOrgel
 by Manuel Hartmann <https://github.com/bAckmumu/swh2017a>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

int servoPin = 6;     // fisrt pin for teh servos

int servoCount = 8;

Servo pipeServos[8];  // create an array of servo objects


void setup() { 
  
  // init servos 
  for(int i=0; i<servoCount; i++)
  {
    pipeServos[i].attach(i+servoPin);
  }
  
}

void loop() {
//  myservo.write(0);
  
  servoTest();
}

void openPipe(int servoNumber) {
  pipeServos[servoNumber].write(90);
}

void closePipe(int servoNumber) {
  pipeServos[servoNumber].write(0);
}

void servoTest() {
  
   for(int s=0; s<servoCount; s++)
   {
      openPipe(s);
      delay(100);

      closePipe(s);
      delay(100);
   }
  
}
