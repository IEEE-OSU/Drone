/* 
 *  This program reads data from both the IMU and the Altimeter
 *  using I2C. 
 *  Author: Aaron Pycraft
 *  Date: 10-31-2016
 */

#include <Wire.h>
#include "SparkFunMPL3115A2.h"

//#define MPU9250_ADDRESS 0x68
#define WHO_AM_I_AK8963  0x00 // should return 
#define WHO_AM_I_MPU9250   0x75 // Should return 0x71

MPL3115A2 Altimeter;


// Define addresses of each unit. 
#define IMU 0x68
#define IMU_WHO_AM_I  0x75
#define xAccelH 0x3B
#define xAccelL 0x3C
#define yAccelH 0x3D     
#define yAccelL 0x3E
#define zAccelH 0x3F
#define zAccelL 0x40
#define imuTempH 0x41
#define imuTempL 0x42
#define xGyroH 0x43
#define xGyroL 0x44
#define yGyroH 0x45 
#define yGyroL 0x46  
#define zGyroH 0x47   
#define zGyroL 0x48  

/* IMU register pins
  address  nane
  HEX DEC  bit[7:0]
  3B  59   ACCEL_XOUT_H[15:8]
  3C  60   ACCEL_XOUT_L[7:0]
  3D  61   ACCEL_YOUT_H R ACCEL_YOUT_H[15:8]
  3E  62   ACCEL_YOUT_L R ACCEL_YOUT_L[7:0]
  3F  63   ACCEL_ZOUT_H R ACCEL_ZOUT_H[15:8]
  40  64   ACCEL_ZOUT_L R ACCEL_ZOUT_L[7:0]
  41  65   TEMP_OUT_H R TEMP_OUT_H[15:8]
  42  66   TEMP_OUT_L R TEMP_OUT_L[7:0]
  43  67   GYRO_XOUT_H R GYRO_XOUT_H[15:8]
  44  68   GYRO_XOUT_L R GYRO_XOUT_L[7:0]
  45  69   GYRO_YOUT_H R GYRO_YOUT_H[15:8]
  46  70   GYRO_YOUT_L R GYRO_YOUT_L[7:0]
  47  71   GYRO_ZOUT_H R GYRO_ZOUT_H[15:8]
  48  72   GYRO_ZOUT_L R GYRO_ZOUT_L[7:0]
  49  73   EXT_SENS_DATA_00 R EXT_SENS_DATA_00[7:0]
  75  117  WHO_AM_I R WHOAMI[7:0]
 */

 /* Altimeter register pins
Register|Address Name|Reset|Reset when STBY to Active|Type|Autoincrement|comment
0x0C  Device Identification Register
(WHO_AM_I) 0xC4 No R 0x0D Fixed Device ID Number
  */

int reading = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  //--Get & print Z acceleration
  //reading = combineBytes(readByte(IMU,zAccelH), readByte(IMU, zAccelL));
  //reading = combineBytes(readByte(IMU,zGyroH), readByte(IMU, zGyroL));
  
  //-- need to combine high byte and low byte to get complete 2 byte value
  //delay(500);
  Serial.print("reading = ");
  //Serial.println(reading);
  Serial.print(readByte(IMU, zGyroH));
  Serial.print(" ");
  Serial.print(readByte(IMU, zGyroL));
  Serial.println();
}

//--Uses wire library to read 1 bytes of data from specified device (address) and 
//  register (subAddress)
uint8_t readByte(uint8_t address, uint8_t subAddress)
{
  uint8_t data; // `data` will store the register data   
  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write(subAddress);                  // Put slave register address in Tx buffer
  Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
  Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address 
  data = Wire.read();                      
  return data;                             
}

//--Combines two bytes to create an int
int combineBytes(uint8_t lsb, uint8_t msb) {
  return lsb | (msb << 8);
}

