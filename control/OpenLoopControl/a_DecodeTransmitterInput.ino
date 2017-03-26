/* a_DecodeTransmitterInput.ino
  
    Author: Iwan Martin
    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft

    Date last modified: February 26, 2017
  
    Objective: Uses arduino Interrupts to decode Tx signal.
  
    INPUTS: Tx PPM signal (Throttle, Roll, Pitch, Yaw)
    OUTPUTS: Scaled tx values within [20, 180]
*/

/* 
 *  The signal from the tx is a PPM signal. The tx sends Th, Pit, Rol, Yaw
 *  on seperate channels as pulses. We can monitor these signals using arduino
 *  interrupts to measure the length of pulse to get the value of the signal. 
 *  
 *  Interrupts can happen at any time. We want to wait for a full cycle of new 
 *  inputs before processing the values. 
 *  
 *  Note: maybe this doesn't matter and we can actually process asynchronously 
 *  - Aaron 2.7.17
*/

  //--Dead zone constant: this  +- scaledInAvg defines dead 
  //  zone range. Inputs in this range will revert back to scaledInAvg.
  const unsigned int kDeadZone = 3;
  
  volatile boolean ISRcomplete; // true at end of a particular tx signal cycle

  //--Values used for tx connection watchdog
  unsigned long int kTxSignalPeriod = 500000; // micro sec; 0.5 seconds
  bool checkForTxSignal;  
  bool txSignalLost;    // true if  
  bool signalReceived;
  unsigned long int timeOfThisInterrupt;
  unsigned long int timeOfLastInterrupt;
  //--For timer & watchdog interrupt. Ideally slightly the period is larger 
  //  than tx signal period
  //--TODO: better documentation of these variables
  const unsigned long kResetTime = 0;
/*
  #define doggieTickle() resetTime = millis();  // This macro will reset the timer
  void(* resetFunc) (void) = 0; //declare reset function @ address 0
*/
  //--Observed limits of the raw tx input signals. Same for each channel.
  const unsigned int pinInMin = 1036;
  const unsigned int pinInMax = 1872;
  //--Desired limits of tx signal (mapped from raw)
  const unsigned int scaledInMin = pinInMin / 4; // 266
  const unsigned int scaledInMax = pinInMax / 4; // 468

// ~A4. Scaled raw input average
//--defines the average "midpoint" between the two scaled input limits.
//  used to center Roll, Pitch and Yaw at 0 eventually.
  const unsigned int scaledInAvg = (scaledInMax + scaledInMin) / 2;

//--The raw input states from the Interrupt Service Routine (ISR)
//  functions. They will eventually be constrained by [pinInMin, pinInMax].
  volatile unsigned int rawInThrottle = 0;
  volatile unsigned int rawInYaw    = 0;
  volatile unsigned int rawInPitch  = 0;
  volatile unsigned int rawInRoll   = 0;

//--Values of throttle, roll, pitch, yaw are determined by length of their 
//  pulse. Measured using the rise/fall time of each ISRs below.
  volatile unsigned int rawInThrottleTime = 0;
  volatile unsigned int rawInYawTime    = 0;
  volatile unsigned int rawInPitchTime  = 0;
  volatile unsigned int rawInRollTime   = 0;

//--Values of tx input after mapping to desired integer range
  /* Replaced by quadSignal global variable
  volatile int scaledInThrottle = 0;
  volatile int scaledInYaw    = 0;
  volatile int scaledInPitch  = 0;
  volatile int scaledInRoll   = 0;
  */
/* 
 *  Setup function to init values used for transmitter decoding. 
 *  Must be called before transmitter decoding will work.
 */
void initTransmitterDecoding() {
  initTxWatchdog();

  //--Setup interrupts for measuring tx signals
  ISRcomplete = false;
  attachAllRisingInterrupts();

  //--Inital values of variables
  /*
  scaledInThrottle = 0;
  scaledInYaw   = 0;
  scaledInPitch = 0;
  scaledInRoll  = 0;
  */
}

//--Watchdog setup
//  code to test killing drone on tx signal loss
void initTxWatchdog() {
  Serial.println("Setting up Tx watchdog...");  
  checkForTxSignal = true;
  //--Setup interrupt to check for valid Tx signal every once-in-a-while
  Timer5.initialize(kTxSignalPeriod);
  Timer5.attachInterrupt(TxSignalCheckISR, kTxSignalPeriod);
}

//--Mutator function to broaden scope of ISRcomplete;
void clearTxCycleFlag() {
  ISRcomplete = false;
}

//--Reads 1 cycle of Tx signals into array
void getTxInput(unsigned int *txSignal ) {  
  checkForSignalLoss();
  
  //--report current signal if no errors exist
  if(ISRcomplete) {
    ISRcomplete = false;
    signalReceived = true;
    processTxSignal(txSignal);
  }

}
/*
 * On every watchdog trigger, check if we have had a full tx signal train. 
 * signal train is measured from throttle to throttle (because throttle 
 * signal is only present if tx signal is uninterrupted)
 */
void checkForSignalLoss() {
  if(checkForTxSignal) { 
    if(signalReceived) {
      TxSignalError = false;
      signalReceived = false;
    } else {
      // TX signal lost
      //--TODO: disable motors
      TxSignalError = true; //flag prevents motors from powering up
    }
    checkForTxSignal = false;
  }
}

/*
 * Map the raw tx signal into the desired range 
 */
void processTxSignal(unsigned int *txSignal) {
    //txSignal[4] = Roll, pitch , throttle, yaw  
    txSignal[0] = deadZone(constrain(rawInRoll / 4, scaledInMin, scaledInMax));  
    txSignal[1] = deadZone(constrain(rawInPitch/4,  scaledInMin, scaledInMax));
    txSignal[2] = constrain(rawInThrottle/4,        scaledInMin, scaledInMax);
    txSignal[3] = deadZone(constrain(rawInYaw / 4,  scaledInMin, scaledInMax));
}

// ~C2.1 deadZone
unsigned int deadZone(unsigned int scaledIn) {
  return (abs(scaledIn - scaledInAvg) <= kDeadZone) ? scaledInAvg: scaledIn;
  /*
  if (abs(scaledIn - scaledInAvg) <= kDeadZone) {
    scaledIn = scaledInAvg;
  }
  return scaledIn;
  */
  
}

// ~C1. Interrupt Functions
/* Functions run asynchronously at rising/falling edges of each tx channel. 
    (or based on a timer period)
    TX input vaules are interpretted from Tx PPM signal. Interrupts must start
    as rising edge triggerred. Attached ISRs will continue until they are detached.

    After a rising edge ISR is triggerred on a pin, the ISR has to setup a
    falling edge ISR on that pin & vice-versa. We measure the time between rising/falling
    edges to determine value of tx input.
    (Can't have a rising/falling ISR simultaneously on a pin)
*/

//--Interrupt service routine for watchdog
void TxSignalCheckISR() {
  checkForTxSignal = true; // trigger flag to process
}

//--Attaches rising edge interrupts to the arduino input pins. The interrupts will
//  be triggerred by the transmitter output signals.
void attachAllRisingInterrupts() {
  attachInterrupt(digitalPinToInterrupt(pinInThrottle), pinThrottleRisingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinInYaw),      pinYawRisingISR,      RISING);
  attachInterrupt(digitalPinToInterrupt(pinInPitch),    pinPitchRisingISR,    RISING);
  attachInterrupt(digitalPinToInterrupt(pinInRoll),     pinRollRisingISR,     RISING);
}

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
  //--not end of pulse train, but throttle disappears if tx signal lost. 
  ISRcomplete = true; 
  /* One full cycle of tx inputs has been read. We can now process the set 
   *  of inputs. We dont have the full set of tx input values until we've 
   *  read one whole cycle of the PPM signal from tx. 
  */
}

// ~C1.4 Yaw - rising and falling edge ISR triggers
void pinYawRisingISR() {  
  rawInYawTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawFallingISR, FALLING);
}

void pinYawFallingISR() {
  rawInYaw = micros() - rawInYawTime;
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawRisingISR, RISING);
  
}

