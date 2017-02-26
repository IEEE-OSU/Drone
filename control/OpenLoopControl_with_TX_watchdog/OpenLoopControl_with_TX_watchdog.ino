/* OpenLoopControl_V2.ino

    Contributors: Roger Kassouf, Iwan Martin, Chen Liang
    Date last modified: February 6, 2017

    Objective: This sketch aims to map real PPM inputs from the receiver, and
    tranform them into real servo outputs to each motor.

    INPUTS: Throttle, Roll, Pitch, Yaw
    OUTPUTS: Speeds of motors 1, 2, 3, 4

*/

/*
    ===============================================================================
    DEFINITIONS AND INCLUSIONS
    ===============================================================================
*/
#include <Servo.h>
/*
    ===============================================================================
*/


/*
    ===============================================================================
    ~A. CONSTANT VARIABLES
    ===============================================================================
    These variables are pre-defined as constants in the sketch. They are used as
    a reference in various points to reduce the amount of math required.
*/

// ~A1. Pins established for raw inputs
/* NOTES: all pins selected are interrupt-enabled pins. They are selected since
    pins 20 and 21 are reserved for the I2C communication protocol.
*/
const unsigned int pinInRoll = 2; // CHANNEL 1
const unsigned int pinInPitch = 3; // CHANNEL 2
const unsigned int pinInThrottle = 18; // CHANNEL 3
const unsigned int pinInYaw = 19; // CHANNEL 4

// ~A2. Limits for raw pin inputs
/* NOTES: variables defined in sections B.1. The selected limits were chosen since
    they are consistent across each input. This creates a two-tailed dead zone at
    the extremes of the stick.
 *  **IMPORTANT** See ~A4., since this value MUST be a whole number (when solved by
    hand using a calculator), the selection of the raw pin limits is what sets it!
*/
const unsigned int pinInMin = 1064;
const unsigned int pinInMax = 1872;

// ~A3. Scaled raw input limits
/* NOTES: variables defined in sections B.#. They are more practica pinIn limits
    since they are adjusted to be the rights scale.
*/
const unsigned int scaledInMin = pinInMin / 4;
const unsigned int scaledInMax = pinInMax / 4;

// ~A4. Scaled raw input average
/* NOTES: This defines the average "midpoint" between the two scaled input limits.
    It will be used to center Roll, Pitch and Yaw at 0 eventually.
*/
const unsigned int scaledInAvg = (scaledInMax + scaledInMin) / 2;

// ~A5. Dead zone constant
/* NOTES: This value +- the scaledInAvg will define the dead zone range. Inputs
    which fall under the range will revert back to scaledInAvg.
*/
const unsigned int deadZoneConstant = 3;

// ~A6. Control constants
/* NOTES: Each constant corresponds to different values inside of the control
    transfer matrix (CTM). Scroll through to see their implementation.

    Descriptions:
    
    ** PARAMETERS TO SELECT **
    speedWidth -- the absolute maximium difference in speed between two motors.
    It must be between 0 and 1, so something like 0.20.
    
    tiltYaw -- the ratio of speeds between tilting (roll and/or pitch) and the
    yaw. Reasonably, you would want this greater than 1, but for sure greater
    than 0. There is no true upper limit.
    
    ** PARAMETERS CALCULATED **
    
    kT -- Apportions part of the Throttle command to the motors.

    kI -- Apportions part of the Roll and Pitch commands to the motors.

    kY -- Apportions part of the Yaw command to the motors.
    
    Tcut -- The throttle speed at which the control transfer will switch on the
    rotational control. When T is less than Tcut, all motors will have the same
    speed.
    
    For a better description of each parameter, look in the controlTransfer function
*/
const float speedWidth=0.2;
const float tiltYaw=2;
const float kY = speedWidth/(1+(tiltYaw/2));
const float kI = (tiltYaw/2)*kY;
const float kT = 4-(2*kI)-kY;
const float TCut=2*speedWidth/kT;

//const float Nmax = sqrt((1 / (4 * kT)) + (1 / (2 * kI*kT)) + (1 / (4 * kY)));
//const float Nmaxsquared = (1 / (4 * kT)) + (1 / (2 * kI*kT)) + (1 / (4 * kY));

// ~A7. Output pins and Servo objects
/* NOTES: Every motor will be given a Servo object correlated to some output pin.
    While the attach will occur in the setup loop, the values here are to be
    pre-defined.

 *  ** IMPORTANT ** Ensure that the pairs of motors (1,3) and (2,4) have these
    properties on the physical craft:
    1. That (1,3) are set to be the same directional rotation, as are (2,4)
    2. That the pairs (1,3) and (2,4) rotate in different directions
    3. That the motors (1,3) are set across from each other on the drone's cross,
    as are (2,4).
    (The established positive direction for the motors is counter-clockwise, and
    originally set for 1 and 3).


 *  ** IMPORTANT ** the output pin selections must be PWM pins. For the Arduino
    Mega, many work, although the interrupt-enabled ones will be skipped.
*/
Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

const unsigned int motor1pin = 5;
const unsigned int motor2pin = 6;
const unsigned int motor3pin = 7;
const unsigned int motor4pin = 8;

// ~A8. Servo limits
/* NOTES: When calibrating the electronic speed controllers (ESCs), it was found
    that an ESC will accept a servo value ranging from 20 to 180. If it is
    possible somehow to increase this, that can be changed here.
*/
const unsigned int servoMin = 20;
const unsigned int servoMax = 180;

/*
    ===============================================================================
*/


/*
    ===============================================================================
    ~B. STATE VARIABLES
    ===============================================================================
    These variables are subject to change, as they are used incrementally
    throughout the sketch.
*/

// ~B1. Raw input state values
/* NOTES: The raw input states from the Interrupt Service Routine (ISR)
    functions. They will eventually be constrained by [pinInMin, pinInMax].
*/
volatile unsigned int rawInRollValue = 0;
volatile unsigned int rawInPitchValue = 0;
volatile unsigned int rawInThrottleValue = 0;
volatile unsigned int rawInYawValue = 0;

// ~B2. Raw input state times
/* NOTES: These are used with a timing routine in the ISR to then determine
    what the value of each variable is.
*/
volatile unsigned int rawInRollTime = 0;
volatile unsigned int rawInPitchTime = 0;
volatile unsigned int rawInThrottleTime = 0;
volatile unsigned int rawInYawTime = 0;

// ~B3. Raw input ISR trigger
/* NOTES: If ISRcomplete == 1, then it's done with its routines.
*/
volatile unsigned int ISRcomplete = 0;

// ~B4. Scaled input state values
/* NOTES: This is just where the rawIn___Value values are stored
    after they are scaled by 4. The roll, pitch, and yaw will
    also be passed through the dead zone function.
*/
unsigned int scaledInRollValue = 0;
unsigned int scaledInPitchValue = 0;
unsigned int scaledInThrottleValue = 0;
unsigned int scaledInYawValue = 0;

// ~B5. Servo output command speed
/* NOTES: The values which will dictate the speed of each motor. The limits
    are defined in A8.
    These are defined at a vector since the output function will be able to
    perform all four calculations within the same iteration. As such, the
    function will point to each cell and update the values.
*/
unsigned int motorsOut[4] = {servoMin, servoMin, servoMin, servoMin};
// Cells in incrememntal order: Motors 1, 2, 3, 4.

/*
    ===============================================================================
*/


/*
    ===============================================================================
    ~C. FUNCTIONS
    ===============================================================================
    All of the functions outside of setup and loop that make the sketch function!
*/

// ~C1. Interrupt Functions
/* NOTES: These functions will be interrupt-enabled, and they will passively
    read each PPM input pin to generate the input values. Note that to begin this
    process, the Rising functions will need to be interrupt-enabled in the setup
    loop. They will continue jutting in the sketch unless the detachInterrupt
    function is called.
*/

// ~C1.1 Roll
void pinRollRisingISR() {
  rawInRollTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInRoll), pinRollFallingISR, FALLING);
}

void pinRollFallingISR() {
  rawInRollValue = micros() - rawInRollTime;
  attachInterrupt(digitalPinToInterrupt(pinInRoll), pinRollRisingISR, RISING);
}

// ~C1.2 Pitch
void pinPitchRisingISR() {
  rawInPitchTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInPitch), pinPitchFallingISR, FALLING);
}

void pinPitchFallingISR() {
  rawInPitchValue = micros() - rawInPitchTime;
  attachInterrupt(digitalPinToInterrupt(pinInPitch), pinPitchRisingISR, RISING);
}

// ~C1.3 Throttle
void pinThrottleRisingISR() {
  rawInThrottleTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInThrottle), pinThrottleFallingISR, FALLING);
}

void pinThrottleFallingISR() {
  rawInThrottleValue = micros() - rawInThrottleTime;
  attachInterrupt(digitalPinToInterrupt(pinInThrottle), pinThrottleRisingISR, RISING);
  ISRcomplete = 1;
}

// ~C1.4 Yaw
void pinYawRisingISR() {
  rawInYawTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawFallingISR, FALLING);
}

void pinYawFallingISR() {
  rawInYawValue = micros() - rawInYawTime;
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawRisingISR, RISING);
  
  
  /* Note: ISRcomplete is set to 1 at the end here, but remember that these
      functions are called asynchonously. The "scaled" set of variables are
      set directly after reading and are used in all following calculations.
      Once all routines are complete, then ISRcomplete can be set to 0 and
      the process can continue.
  */
}

// ~C2. Scaling and Mapping Functions
/* NOTES: The purpose of these functions is to assist in computing the scaled
    input values, after the raw input values have been read.
*/

// ~C2.1 deadZone
unsigned int deadZone(unsigned int scaledInValue, unsigned int deadZoneConstant) {
  if (abs(scaledInValue - scaledInAvg) <= deadZoneConstant) {
    scaledInValue = scaledInAvg;
  }
  return scaledInValue;
}

// ~C2.2 mapIntToFloat
float mapIntToFloat(int x, int in_min, int in_max, float out_min, float out_max)
{
  // Cast the input limit integers to floats
  x = (float) x; in_min = (float) in_min; in_max = (float) in_max;
  // Perform the map, return the float
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ~C2.3 mapFloatToInt
int mapFloatToInt(float x, float in_min, float in_max, int out_min, int out_max)
{
  // We will perform the math in floats first. Cast output limit integers to floats.
  out_min = (float) out_min; out_max = (float) out_max;
  // Perform the map (using floats!)
  float x_out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  // Cast the output variable as an integer. While this does perform an inherent
  // "rounding down" it should be reasonably close to the desired value.
  x_out = (int) x_out;
  // Return the integer
  return x_out;
}

// ~C3 Meta-functions
/* NOTES: Meta-functions use other functions to accomplish a broader goal.
*/

// ~C3.1 scaleValues
void scaleValues() {
  scaledInRollValue = deadZone(constrain(rawInRollValue / 4, scaledInMin, scaledInMax), deadZoneConstant);
  scaledInPitchValue = deadZone(constrain(rawInPitchValue / 4, scaledInMin, scaledInMax), deadZoneConstant);
  scaledInThrottleValue = constrain(rawInThrottleValue / 4, scaledInMin, scaledInMax);
  scaledInYawValue = deadZone(constrain(rawInYawValue / 4, scaledInMin, scaledInMax), deadZoneConstant);
}

// ~C3.2 controlTransfer
void controlTransfer(unsigned int scaledInRoll, unsigned int scaledInPitch, unsigned int scaledInThrottle, unsigned int scaledInYaw) {
  // Step 1: Convert the scaledIn values to values on a signed/unsigned binary basis:
  float R = mapIntToFloat(scaledInRoll, scaledInMin, scaledInMax, -1, 1);
  float P = mapIntToFloat(scaledInPitch, scaledInMin, scaledInMax, -1, 1);
  float U = mapIntToFloat(scaledInThrottle, scaledInMin, scaledInMax, 0, 1);
  float Y = mapIntToFloat(scaledInYaw, scaledInMin, scaledInMax, -1, 1);
      
  // Step 2: Perform the CTM calculations for each motor        
  float N1 = 0;
  float N2 = 0;
  float N3 = 0;
  float N4 = 0;
  float NT = (kT*U/4);
  float NR = (kI*R/2);
  float NP = (kT*P/2);
  float NY = (kY*Y/4);
  if(U <= TCut) {
       N1 = NT;
       N2 = NT;
       N3 = NT;
       N4 = NT;
  }
  else{
       N1 = NT + NR + NY;
       N2 = NT + NP - NY;
       N3 = NT - NR + NY;
       N4 = NT - NP - NY;
  }

  // Step 3: Constrain the squared speeds to [0, (Nmax)^2]
  N1 = constrain(N1, 0, 1);
  N2 = constrain(N2, 0, 1);
  N3 = constrain(N3, 0, 1);
  N4 = constrain(N4, 0, 1);
  // Step 5: Re-map the speed values to each motor
  motorsOut[0] = mapFloatToInt(N1, 0, 1, servoMin, servoMax);
  motorsOut[1] = mapFloatToInt(N2, 0, 1, servoMin, servoMax);
  motorsOut[2] = mapFloatToInt(N3, 0, 1, servoMin, servoMax);
  motorsOut[3] = mapFloatToInt(N4, 0, 1, servoMin, servoMax);
}

// ~C3.3 powerMotors
void powerMotors() {
  motor1.write(motorsOut[0]);
  motor2.write(motorsOut[1]);
  motor3.write(motorsOut[2]);
  motor4.write(motorsOut[3]);
}

// ~C3.4 attachAllInterrupts
void attachAllRisingInterrupts() {
  attachInterrupt(digitalPinToInterrupt(pinInRoll), pinRollRisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinInPitch), pinPitchRisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinInThrottle), pinThrottleRisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawRisingISR, RISING);
}

// ~C3.5 printResults
void printResults() {
  Serial.print("Throttle: ");
  Serial.print(scaledInThrottleValue);
  Serial.print("\t");
  Serial.print("Roll: ");
  Serial.print(scaledInRollValue);
  Serial.print("\t");
  Serial.print("Pitch: ");
  Serial.print(scaledInPitchValue);
  Serial.print("\t");
  Serial.print("Yaw: ");
  Serial.print(scaledInYawValue);
  /*
  Serial.print("\t");
  Serial.print("Motor 1: ");
  Serial.print(motorsOut[0]);
  Serial.print("\t");
  Serial.print("Motor 2: ");
  Serial.print(motorsOut[1]);
  Serial.print("\t");
  Serial.print("Motor 3: ");
  Serial.print(motorsOut[2]);
  Serial.print("\t");
  Serial.print("Motor 4: ");
  Serial.print(motorsOut[3]);
  */
  Serial.println();
}
/*
    ===============================================================================
*/


/*
    ===============================================================================
    ~D. LOOPS
    ===============================================================================
    Where all of the code is executed.
*/

//#include "TimerOne.h"
#include "TimerFive.h"

unsigned long int kTxSignalPeriod = 500000;
//--interrupt handling features
bool watchDogTriggered;
bool signalReceived;

void setup() {
  attachAllRisingInterrupts();
  
//  motor1.attach(motor1pin);
//  motor2.attach(motor2pin);
  motor3.attach(motor3pin);
//  motor4.attach(motor4pin);
  
  Serial.begin(38400);

  // code to test killing drone on tx signal loss
  watchDogTriggered = false;
  
  Timer5.initialize(kTxSignalPeriod );         // initialize timer1, and set a 1/2 second period
  //Timer5.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
  Timer5.attachInterrupt(callback, kTxSignalPeriod );  // attaches callback() as a timer overflow interrupt

}


void callback()
{
  // temporary, for testing
  //while(true) {
 
  //  Serial.print("Trapped!");
 //   Serial.println("\t " + micros());
  //}
  // trigger flag to process
  watchDogTriggered = true;
  
  //check this somewhere 
}

void loop() {
  if(watchDogTriggered){
        if(signalReceived){
          signalReceived = false;
        } else {
          Serial.print("Watchdog triggered");
          Serial.println(micros());
          
          motor1.write(20);          
          motor2.write(20);
          motor3.write(20);
          motor4.write(20);
          
        }
        watchDogTriggered = false;
  }
  
  if (ISRcomplete == 1) {
    signalReceived = true;     
    
    scaleValues();
    controlTransfer(scaledInRollValue, scaledInPitchValue, scaledInThrottleValue, scaledInYawValue);
    powerMotors();
    printResults();
    ISRcomplete = 0;
  }

}
