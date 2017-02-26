/* Practice reading IMU for drone project
 *  Author: Aaron Pycraft
 *  Date: 02-26-2017
 *  Heavily borrowed from: sparkfun MPU2250BasicsAHRS
 */

/*
 SDA and SCL should have external pull-up resistors (to 3.3V).
 10k resistors are on the EMSENSR-9250 breakout board.
 
 Hardware setup:
 MPU9250 Breakout --------- Arduino
 VDD ----------------------- 3.3V
 SDA ----------------------- arduino SDA
 SCL ----------------------- arduino SCL
 GND ----------------------- GND
*/
/**
#include <Wire.h>

int imuReading= 0;
#define MPU9250_ADDRESS 0x68
#define WHO_AM_I_AK8963  0x00 // should return 
#define WHO_AM_I_MPU9250   0x75 // Should return 0x71

const uint8_t accelZ = 0x3F; //high byte
*/
