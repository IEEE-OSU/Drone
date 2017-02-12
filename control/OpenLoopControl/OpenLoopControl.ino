/* OpenLoopControl.ino

    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft
    Date last modified: February 7, 2017

    Objective: This sketch maps PPM inputs from the receiver to servo outputs for each motor.

    INPUTS: Throttle, Roll, Pitch, Yaw
    OUTPUTS: Speeds of motors 1, 2, 3, 4
*/

//--Note: arduino file complication is alphabetical.

// Constants
#include <Servo.h>
#include "PinDefinitions.h"

//--variables declared elsewhere
extern unsigned int scaledInRoll, scaledInPitch, scaledInThrottle, scaledInYaw;
extern volatile boolean ISRcomplete;

//--Setup to run once. 
void setup() {
  attachAllRisingInterrupts();
  attachAllMotors();
  Serial.begin(38400);
}

//--Main program loop
void loop() {
  if (ISRcomplete) {
    scaleValues();
    controlTransfer(scaledInRoll, scaledInPitch, scaledInThrottle, scaledInYaw);
    powerMotors();
    printResults();
    ISRcomplete = false;
  }
}


