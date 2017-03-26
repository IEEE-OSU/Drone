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
#include <SPI.h> 
#include <Wire.h>
#include "quaternionFilters.h"
#include "MPU9250.h"
#include <Adafruit_GFX.h>
//#include <Adafruit_PCD8544.h> for lcd... unnecessary

//--variables declared elsewhere
extern unsigned int scaledInRoll, scaledInPitch, scaledInThrottle, scaledInYaw;
extern volatile boolean ISRcomplete;

//--Setup to run once. 
void setup() {
  IMUsetup();
  
  Serial.begin(38400);
  attachAllRisingInterrupts();
  //attachAllMotors();
  
  
}

//--Main program loop
void loop() {
  if (ISRcomplete) {
    //scaleValues();
    //controlTransfer(scaledInRoll, scaledInPitch, scaledInThrottle, scaledInYaw);
    //powerMotors();
    //printResults();
/*
 * 
 * 
 */
  Serial.print("Throttle: " + scaledInThrottle + '\t');
  Serial.print("Roll: "     + scaledInRoll + '\t');
  Serial.print("Pitch: "    + scaledInPitch + '\t');
  Serial.print("Yaw: "      + scaledInYaw + '\t');
  //Serial.print("Motor 1: " + motorsOut[0] + '\t');
 // Serial.print("Motor 2: " + motorsOut[1] + '\t');
 // Serial.print("Motor 3: " + motorsOut[2] + '\t');
  //Serial.print("Motor 4: " + motorsOut[3] + '\n');
   /* 
    *  
    */
    ISRcomplete = false;
  }
}


