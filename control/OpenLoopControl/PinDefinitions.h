#ifndef PIN_DEF_H
  #define PIN_DEF_H
 
  /* 
   *  NOTES: all pins selected are interrupt-enabled pins. They are selected 
   *  since pins 20 and 21 are reserved for the I2C communication protocol.
  */
 
  // Arduino pins that connect to transmitter. Must be interrupt-enabled. 
  const unsigned int pinInRoll = 2;       // tx output CHANNEL 1
  const unsigned int pinInPitch = 3;      // tx output CHANNEL 2
  const unsigned int pinInThrottle = 18;  // tx output CHANNEL 3
  const unsigned int pinInYaw = 19;       // tx output CHANNEL 4

  // Arduino pins that connect to ESC servo data inputs (white wire)
  #ifndef MOTOR_PINS
    #define MOTOR_PINS
    const unsigned int motor1pin = 5;
    const unsigned int motor2pin = 6;
    const unsigned int motor3pin = 7;
    const unsigned int motor4pin = 8;
  #endif
 
#endif
