/* OpenLoopControl.ino

    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft

    Date last modified: February 26, 2017

    Objective: This sketch maps PPM inputs from the receiver to servo outputs for each motor.

    INPUTS: Throttle, Roll, Pitch, Yaw
    OUTPUTS: Speeds of motors 1, 2, 3, 4
*/

#include <Servo.h>
#include "PinDefinitions.h"
#include <Wire.h>

//--Global variables
  //--Used as main control signal from tx input to motor speed settings. 
  unsigned int *quadSignal; 
                        

//--Setup to run once. 
void setup() {
  Wire.begin();
  Serial.begin(38400);
  quadSignal = new unsigned int[4];
  clearTxCycleFlag(); // true at the end of each tx signal cycle
  initTransmitterDecoding();
  initMotorControl();
}

//--Main program loop
void loop() {

  //--Get input from transmitter
  getTxInput(quadSignal);
  printTxSignals(quadSignal);
  //--TODO: shut down program if no signal recieved from tx (like if the tx was off)

  //--Perform necessary processing
  processTxSignal(quadSignal);
  
  //--Send signal to motors
  powerMotors(quadSignal);
  //printMotorValues(quadSignal);
 }

 void destroy() {
  delete quadSignal;
 }

//--Prints processed TX input values.
void printTxSignals(unsigned int txSignal[4]) {
  Serial.print("Throttle: " + txSignal[2] + '\t');
  Serial.print("Yaw: "      + txSignal[3] + '\t');
  Serial.print("Pitch: "    + txSignal[1] + '\t');
  Serial.print("Roll: "     + txSignal[0] + '\t');
}


//--Prints motor output values.
void printMotorValues(unsigned int motorsOut[4]) { 
  Serial.print("Motor 1: " + motorsOut[0] + '\t');
  Serial.print("Motor 2: " + motorsOut[1] + '\t');
  Serial.print("Motor 3: " + motorsOut[2] + '\t');
  Serial.print("Motor 4: " + motorsOut[3] + '\n');
}

