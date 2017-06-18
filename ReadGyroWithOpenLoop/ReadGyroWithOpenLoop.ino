/* OpenLoopControl.ino

    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft

    Date last modified: February 26, 2017

    Objective: This sketch maps PPM inputs from the receiver to servo outputs 
               for each motor.

    INPUTS: Throttle, Roll, Pitch, Yaw
    OUTPUTS: Speeds of motors 1, 2, 3, 4
*/

#include <Servo.h>
#include <Wire.h>
#include <TimerOne.h>
#include "PinDefinitions.h"
#include <SparkFunMPU9250-DMP.h>


//--Global variables
  //--Used as main control signal from tx input to motor speed settings. 
  unsigned int *quadSignal; //although this is global scope; we'll pass as argument
  unsigned int *motorSpeeds; // used as input to motor control
  const bool ENABLE_MOTORS = true;
  bool TxSignalError = false; //true if error exists in Tx
  bool skipControlTransfer = false;
  // Definitions of indexes in txSignal. ki for "const index"
  const int THROTTLE = 2; 
  const int PITCH    = 1;
  const int ROLL     = 0;
  const int YAW      = 3;

  float gyroX = 0;
  float gyroY = 0;
  float gyroZ = 0;
  MPU9250_DMP imu;  
                        
//--Setup to run once. 
void setup() {
  Serial.begin(115200);

  configIMU();
  
  //--Setup transmitter decoding
  clearTxCycleFlag(); // true at the end of each tx signal cycle
  initTransmitterDecoding();
  


  quadSignal = new unsigned int[4];
  motorSpeeds = new unsigned int[4];
  
  if(ENABLE_MOTORS) {
    initMotorControl();
    setMotorsToMin();
  }
  while(millis() < 16000){
    // Wait until Gyro has finished calibrating
  }
  
  Serial.println("setup complete");
}

//--Main program loop
void loop() {
  //--Get input from transmitter
  getTxInput(quadSignal);
  if(TxSignalError) {
    Serial.print("Tx signal lost!..."); Serial.println(micros());
    setMotorsToMin();
    skipControlTransfer = true;
  } else  {
    //printTxSignals(quadSignal); // uncomment to see tx signals
  }

  if(!skipControlTransfer){
  // Update based on IMU
    if ( imu.fifoAvailable() )
  {
    // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    {
      updateGyro(); // Get updated Gyro data
      printGyroValues(); // Uncomment to print raw gyro values
    }
    else {
    Serial.print("DMP Update Fifo error"); Serial.println(micros());
    }
  }
    
  }
   
  //--Tranform Tx signal to motor speed settings.
  //--TODO: implement new control algorithm
  if(skipControlTransfer) {
    skipControlTransfer = false;
  } else {
    controlTransfer(quadSignal, motorSpeeds); 
  }
  //printMotorValues(motorSpeeds);
  
  //--Send signal to motors, if no errors exist && they're enabled
  //--TODO; check that this logical check is working correctly
  if(!TxSignalError && ENABLE_MOTORS) {
    powerMotors(motorSpeeds);
  }
}

//--Probably unnecessary b/c quad will rarely be shutdown in a civil manner.
//  included for best practice.
void destroy() {
  delete quadSignal;
}

//--Prints processed TX input values.
void printTxSignals(unsigned int *txSignal) {
  //--Note: something messes up character encoding when trying to print 
  //  different data types at once. To print, follow following format:
  Serial.print("Throttle: "); Serial.print(txSignal[THROTTLE]); Serial.print("\t");
  Serial.print("Roll: \t");   Serial.print(txSignal[ROLL]);     Serial.print("\t");
  Serial.print("Pitch: ");    Serial.print(txSignal[PITCH]);    Serial.print("\t");
  Serial.print("Yaw: \t");    Serial.print(txSignal[YAW]);      Serial.print("\t");
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

//--Utility to quickly set values to zero
void clearQuadSignal() {
  quadSignal[0] = 0;
  quadSignal[1] = 0;
  quadSignal[2] = 0;
  quadSignal[3] = 0;
}


void printGyroValues(){
    
    Serial.println("Gyro: " + String(gyroX) + ", " +
              String(gyroY) + ", " + String(gyroZ) + " dps");
}

