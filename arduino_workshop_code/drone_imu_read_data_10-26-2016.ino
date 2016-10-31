/* Practice reading IMU for drone project
 *  Author: Aaron Pycraft
 *  Date: 10-26-2016
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
#include <Wire.h>

int reading = 0;
#define MPU9250_ADDRESS 0x68
#define WHO_AM_I_AK8963  0x00 // should return 
#define WHO_AM_I_MPU9250   0x75 // Should return 0x71

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);// must be 9600 or serial output is garbage 
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t accelZ = 0x3F; //high byte
  uint8_t reading = readByte(MPU9250_ADDRESS, accelZ);
  //-- need to combind high byte and low byte to get complete 2 byte value
  
  delay(500);
  Serial.print("reading = ");
  Serial.println(reading);
}
/* 
 *  Returns 1 byte of data from the specified device and address.
 *  
 */
uint8_t readByte(uint8_t address, uint8_t subAddress)
{
  uint8_t data; // `data` will store the register data   
  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write(subAddress);                  // Put slave register address in Tx buffer
  Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
  Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address 
  data = Wire.read();                      // Fill Rx buffer with result
  return data;                             // Return data read from slave register
}

/* 
 *  Other abstract function TBD
 */


