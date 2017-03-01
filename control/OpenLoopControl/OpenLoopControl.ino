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
  const bool runMotors = false;
                        

//--Setup to run once. 
void setup() {
  //Wire.begin();
  Serial.begin(38400);
  quadSignal = new unsigned int[4];
  clearTxCycleFlag(); // true at the end of each tx signal cycle
  initTransmitterDecoding();
  //initMotorControl();

  Serial.println("setup complete");
  
  
}

//--Main program loop
void loop() {
  //--Get input from transmitter
  getTxInput(quadSignal);
  printTxSignals(quadSignal);

  //--Perform necessary processing
  processTxSignal(quadSignal);
  controlTransfer(quadSignal);
  
  //--Send signal to motors
  if(runMotors) {
    powerMotors(quadSignal);
    printMotorValues(quadSignal);
  }
 }

 void destroy() {
  delete quadSignal;
 }

//--Prints processed TX input values.
void printTxSignals(unsigned int *txSignal) {
  //--Note: something messes up character encoding when trying to print different
  //  data types at once. To print, follow following format:
  Serial.print("Throttle: "); Serial.print(txSignal[2]); Serial.print('\t');
  Serial.print("Yaw: ");      Serial.print(txSignal[3]); Serial.print('\t');
  Serial.print("Pitch: ");    Serial.print(txSignal[1]); Serial.print('\t');
  Serial.print("Roll: ");     Serial.print(txSignal[0]); Serial.print('\t');
  Serial.println();
}

//--Prints motor output values.
void printMotorValues(unsigned int *motorsOut) { 
  Serial.print("Motor 1: "); Serial.print(motorsOut[0]); Serial.print('\t');
  Serial.print("Motor 2: "); Serial.print(motorsOut[1]); Serial.print('\t');
  Serial.print("Motor 3: "); Serial.print(motorsOut[2]); Serial.print('\t');
  Serial.print("Motor 4: "); Serial.print(motorsOut[3]); Serial.print('\t');
  Serial.println();
}

