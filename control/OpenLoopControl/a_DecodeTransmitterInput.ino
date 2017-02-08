/* a_DecodeTransmitterInput.ino

    Author: Iwan Martin
    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft
    Date last modified: February 7, 2017

    Objective: Uses arduino Interrupts to decode Tx signal.

    INPUTS: Tx PPM signal (Throttle, Roll, Pitch, Yaw)
    OUTPUTS: Scaled tx values within [20, 180]
*/
extern unsigned int motorsOut[];

  /* The signal from the tx is a PPM signal. The tx sends throttle, pitch, roll, yaw 
   *  on seperate channels as pulses. We can monitor these signals using arduino
   *  interrupts to measure the length of pulse to get the value of the signal. 
   *  
   *  Interrupts can happen at any time. We want to wait for a full cycle of new 
   *  inputs before processing the values. 
   *  
   *  Note: maybe this doesn't matter and we can actually process asynchronously 
   *  - Aaron 2.7.17
   */

// ~A2. Limits for pin raw inputs
/* NOTES: variables defined in sections B.1. The selected limits were chosen since
    they are consistent across each input. This creates a two-tailed dead zone at
    the extremes of the stick.
 *  **IMPORTANT** See ~A4., since this value MUST be a whole number (when solved by
    hand using a calculator), the selection of the raw pin limits is what sets it!
*/
const unsigned int pinInMin = 1064;
const unsigned int pinInMax = 1872;


// Transmitter inputs
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



// ~B4. Scaled input state values
/* NOTES: This is just where the rawIn___Value values are stored
    after they are scaled by 4. The roll, pitch, and yaw will
    also be passed through the dead zone function.
*/
#ifndef SCALED_TX_VALS
  #define SCALED_TX_VALS
  unsigned int scaledInRollValue = 0;
  unsigned int scaledInPitchValue = 0;
  unsigned int scaledInThrottleValue = 0;
  unsigned int scaledInYawValue = 0;
#endif



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

/* 
  File contains functions necessary for tx signal processing (using interrupts)
*/

// ~B3. Raw input ISR trigger
/* NOTES: If ISRcomplete == 1, then it's done with its routines.
*/
volatile boolean ISRcomplete = false;


// ~C1. Interrupt Functions
/* NOTES: These functions will be interrupt-enabled, and they will passively
    read each PPM input pin to generate the input values. Note that to begin this
    process, the Rising functions will need to be interrupt-enabled in the setup
    loop. They will continue jutting in the sketch unless the detachInterrupt
    function is called.
*/

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
}

// ~C1.4 Yaw - rising and falling edge ISR triggers
void pinYawRisingISR() {
  rawInYawTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawFallingISR, FALLING);
}

void pinYawFallingISR() {
  rawInYawValue = micros() - rawInYawTime;
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawRisingISR, RISING);
  ISRcomplete = true;
  /* Note: ISRcomplete is set true here. One full cycle of tx inputs has been read. 
   *  We can now process the set of inputs. We dont have the full set of tx input 
   *  values until we've read one whole cycle of the PPM signal from tx. 
  */
}


//--Attaches rising edge interrupts to the arduino input pins. The interrupts will
//  be triggerred by the transmitter output signals.
void attachAllRisingInterrupts() {
  attachInterrupt(digitalPinToInterrupt(pinInRoll), pinRollRisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinInPitch), pinPitchRisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinInThrottle), pinThrottleRisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawRisingISR, RISING);
}

//--Prints processed TX input values and motor output values.
void printResults() {
  Serial.print("Throttle: " + scaledInThrottleValue + '\t');
  Serial.print("Roll: "     + scaledInRollValue + '\t');
  Serial.print("Pitch: "    + scaledInPitchValue + '\t');
  Serial.print("Yaw: "      + scaledInYawValue + '\t');
  
  Serial.print("Motor 1: " + motorsOut[0] + '\t');
  Serial.print("Motor 2: " + motorsOut[1] + '\t');
  Serial.print("Motor 3: " + motorsOut[2] + '\t');
  Serial.print("Motor 4: " + motorsOut[3] + '\n');
}


