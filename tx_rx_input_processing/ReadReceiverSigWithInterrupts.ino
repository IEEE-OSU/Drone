// file:
// author: Iwan Martin
// date:
// edited: aaron pycraft
// Synopsis: 
//  This program takes interrupt data from the 4 interrupt pins (2, 3, 18, & 19)
//   prints them and writes them to the pins 5, 6, 7, and 8
// The intended configuration is as follows:
//   chn 1 - 2   - Roll
//   chn 4 - 19  - Yaw
//   chn 3 - 18  - Throttle
//   chn 2 - 3   - Pitch
//   Note: these are in order of transmission

// NOTE: About interrupts. 
// 2, 3, 18, 19, 20, 21 are interrupt enabled
// 20, 21 are needed for the I2C communication protocal
// So we'll be usign 2, 3, 18, and 19, for receivers.

#include <Servo.h> 

// Pin interrupt variables
volatile unsigned int pin2Raw = 0;
volatile unsigned long pin2Time = 0;
volatile unsigned int pin3Raw = 0;
volatile unsigned long pin3Time = 0;
volatile unsigned int pin18Raw = 0;
volatile unsigned long pin18Time = 0;
volatile unsigned int pin19Raw = 0;
volatile unsigned long pin19Time = 0;

volatile unsigned int roll = 0; // interpretted tx signals
volatile unsigned int pitch = 0;
volatile unsigned int throttle = 0;
volatile unsigned int yaw = 0;

static unsigned int INPUT_MAX = 468; // limits of tx input signal
static unsigned int INPUT_MIN = 266;
static unsigned int CONTROL_MIN = 20; // limits; roll, pitch yaw, throttle 
static unsigned int CONTROL_MAX = 180;

volatile int processVals = 0;

void writeOutputs();
void convertRaws();
int processPinInput(int value);

Servo motorA;
Servo motorB;
Servo motorC;
Servo motorD;


void setup() {
  attachInterrupt(digitalPinToInterrupt(2), pin2RisingISR, RISING);  
  attachInterrupt(digitalPinToInterrupt(3), pin3RisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(18), pin18RisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(19), pin19RisingISR, RISING);

  motorA.attach(5);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(processVals == 1){
  convertRaws();
  writeOutputs();
    
  Serial.print("throttle: ");
  Serial.print(throttle);
  Serial.print(" pitch: ");
  Serial.print(pitch);
  Serial.print(" roll: ");
  Serial.print(roll);
  Serial.print(" yaw: ");
  Serial.println(yaw);
  processVals = 0;
  }
    
}

void convertRaws(){
    roll = processPinInput(pin2Raw);
    throttle = processPinInput(pin18Raw);
    pitch = processPinInput(pin3Raw);
    yaw = processPinInput(pin19Raw);
}

int processPinInput(unsigned int value){
  int processedInput;
  int newVal = constrain(value / 4, INPUT_MIN, INPUT_MAX);
  processedInput = map(newVal, INPUT_MIN, INPUT_MAX, 20, 180);
  return processedInput;
}

void writeOutputs(){
  motorA.write(throttle);
  motorB.write(yaw);
}


/*
 * ==================================================================
 * Interrupt Service Routines
 * ==================================================================
 */

void pin2RisingISR(){
  pin2Time = micros();
  attachInterrupt(digitalPinToInterrupt(2), pin2FallingISR, FALLING);
}

void pin2FallingISR(){
  pin2Raw = micros() - pin2Time;

  attachInterrupt(digitalPinToInterrupt(2), pin2RisingISR, RISING);  
}


void pin3RisingISR(){
  pin3Time = micros();
  attachInterrupt(digitalPinToInterrupt(3), pin3FallingISR, FALLING);  
}

void pin3FallingISR(){
  pin3Raw = micros() - pin3Time;
  attachInterrupt(digitalPinToInterrupt(3), pin3RisingISR, RISING);  
}


void pin18RisingISR(){
  pin18Time = micros();
  attachInterrupt(digitalPinToInterrupt(18), pin18FallingISR, FALLING);  
}
void pin18FallingISR(){
  pin18Raw = micros() - pin18Time;
  attachInterrupt(digitalPinToInterrupt(18), pin18RisingISR, RISING);  
}


void pin19RisingISR(){
  pin19Time = micros();
  attachInterrupt(digitalPinToInterrupt(19), pin19FallingISR, FALLING);  
}
void pin19FallingISR(){
  pin19Raw = micros() - pin19Time;
  processVals = 1;
  attachInterrupt(digitalPinToInterrupt(19), pin19RisingISR, RISING);  
}
  

