/* 
  File contains functions necessary for tx signal processing (using interrupts)
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

// ~C1.4 Yaw
void pinYawRisingISR() {
  rawInYawTime = micros();
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawFallingISR, FALLING);
}

void pinYawFallingISR() {
  rawInYawValue = micros() - rawInYawTime;
  attachInterrupt(digitalPinToInterrupt(pinInYaw), pinYawRisingISR, RISING);
  ISRcomplete = 1;
  /* Note: ISRcomplete is set to 1 at the end here, but remember that these
      functions are called asynchonously. The "scaled" set of variables are
      set directly after reading and are used in all following calculations.
      Once all routines are complete, then ISRcomplete can be set to 0 and
      the process can continue.
  */
}
