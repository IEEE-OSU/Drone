/* a_DecodeTransmitterInput.ino
  
    Author: Iwan Martin
    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft
    Date last modified: February 26, 2017
  
    Objective: Uses arduino Interrupts to decode Tx signal.
  
    INPUTS: Tx PPM signal (Throttle, Roll, Pitch, Yaw)
    OUTPUTS: Scaled tx values within [20, 180]
*/

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
  volatile boolean ISRcomplete; //--denotes the end of a particular tx signal cycle. 

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
// practical pinIn limits; adjusted to be the rights scale.
  const unsigned int scaledInMin = pinInMin / 4;
  const unsigned int scaledInMax = pinInMax / 4;

// ~A4. Scaled raw input average
//--defines the average "midpoint" between the two scaled input limits.
// It will be used to center Roll, Pitch and Yaw at 0 eventually.
  const unsigned int scaledInAvg = (scaledInMax + scaledInMin) / 2;

// ~B4. Scaled input state values
/* NOTES: This is just where the rawIn___Value values are stored
    after they are scaled by 4. The roll, pitch, and yaw will
    also be passed through the dead zone function.
*/

//--The raw input states from the Interrupt Service Routine (ISR)
//  functions. They will eventually be constrained by [pinInMin, pinInMax].
  volatile unsigned int rawInThrottle = 0;
  volatile unsigned int rawInYaw = 0;
  volatile unsigned int rawInPitch = 0;
  volatile unsigned int rawInRoll = 0;

//--Values of throttle, roll, pitch, yaw are determined by length of their 
//  pulse. Measured using the rise/fall time of each ISRs below.
  volatile unsigned int rawInThrottleTime = 0;
  volatile unsigned int rawInYawTime = 0;
  volatile unsigned int rawInPitchTime = 0;
  volatile unsigned int rawInRollTime = 0;

//--Values of tx input after mapping to desired integer range
  volatile int scaledInThrottle = 0;
  volatile int scaledInYaw = 0;
  volatile int scaledInPitch = 0;
  volatile int scaledInRoll = 0;

/* 
 *  Setup function to init values used for transmitter decoding. 
 *  Must be called before transmitter decoding will work.
 */
void initTransmitterDecoding() {
  scaledInThrottle = 0;
  scaledInYaw = 0;
  scaledInPitch = 0;
  scaledInRoll = 0;
  ISRcomplete = false;
  attachAllRisingInterrupts();
}

//--Mutator function to broaden scope of ISRcomplete;
void clearTxCycleFlag() {
  ISRcomplete = false;
}
//--Accessor function for ISRcomplete
boolean isTxSignalCycleComplete() {
  return ISRcomplete;
}

//--Reads 1 cycle of Tx signals into array
void getTxInput(unsigned int txSignal[4]) {
  if(ISRcomplete) {
    //txSignal[4] = Roll, pitch , throttle, yaw  
    txSignal[0] = scaledInRoll;
    txSignal[1] = scaledInPitch;
    txSignal[2] = scaledInThrottle;
    txSignal[3] = scaledInYaw;

    ISRcomplete = false;
  }
}

//--Attaches rising edge interrupts to the arduino input pins. The interrupts will
//  be triggerred by the transmitter output signals.
void attachAllRisingInterrupts() {
  attachInterrupt(digitalPinToInterrupt(pinInThrottle), pinThrottleRisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinInYaw),    pinYawRisingISR,    RISING);
  attachInterrupt(digitalPinToInterrupt(pinInPitch),  pinPitchRisingISR,  RISING);
  attachInterrupt(digitalPinToInterrupt(pinInRoll),   pinRollRisingISR,   RISING);
}

// ~C1. Interrupt Functions
/* Functions run asynchronously at rising/falling edges of each tx channel.
    TX input vaules are interpretted from Tx PPM signal. Interrupts must start
    as rising edge triggerred. Attached ISRs will continue until they are detached.

    After a rising edge ISR is triggerred on a pin, the ISR has to setup a
    falling edge ISR on that pin & vice-versa. We measure the time between rising/falling
    edges to determine value of tx input.
    (Can't have a rising/falling ISR simultaneously on a pin)
*/

// ~C1.1 Roll
void pinRollRisingISR() {
  rawInRollTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInRoll), pinRollFallingISR, FALLING);
}

void pinRollFallingISR() {
  rawInRoll = micros() - rawInRollTime;
  attachInterrupt(digitalPinToInterrupt(pinInRoll), pinRollRisingISR, RISING);
}

// ~C1.2 Pitch
void pinPitchRisingISR() {
  rawInPitchTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInPitch), pinPitchFallingISR, FALLING);
}

void pinPitchFallingISR() {
  rawInPitch = micros() - rawInPitchTime;
  attachInterrupt(digitalPinToInterrupt(pinInPitch), pinPitchRisingISR, RISING);
}

// ~C1.3 Throttle
void pinThrottleRisingISR() {
  rawInThrottleTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInThrottle), pinThrottleFallingISR, FALLING);
}

void pinThrottleFallingISR() {
  rawInThrottle = micros() - rawInThrottleTime;
  attachInterrupt(digitalPinToInterrupt(pinInThrottle), pinThrottleRisingISR, RISING);
}

// ~C1.4 Yaw - rising and falling edge ISR triggers
void pinYawRisingISR() {
  rawInYawTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawFallingISR, FALLING);
}

void pinYawFallingISR() {
  rawInYaw = micros() - rawInYawTime;
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawRisingISR, RISING);
  ISRcomplete = true;
  /* One full cycle of tx inputs has been read. We can now process the set 
   *  of inputs. We dont have the full set of tx input values until we've 
   *  read one whole cycle of the PPM signal from tx. 
  */
}

