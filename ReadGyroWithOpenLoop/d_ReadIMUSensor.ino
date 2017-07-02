
/*
  IMPORTANT:
  SDA and SCL should have external pull-up resistors (to 3.3V).
  10k resistors are on the EMSENSR-9250 breakout board.
 
 Hardware setup:
 MPU9250 Breakout --------- Arduino
 VDD ----------------------- 3.3V
 SDA ----------------------- arduino SDA
 SCL ----------------------- arduino SCL
 GND ----------------------- GND
*/

void configIMU(){
  
  imu.setSensors(INV_XYZ_GYRO); // Enable gyroscope 

  // Use setGyroFSR() and setAccelFSR() to configure the
  // gyroscope and accelerometer full scale ranges.
  // Gyro options are +/- 250, 500, 1000, or 2000 dps
  imu.setGyroFSR(250); // Set gyro to 2000 dps

// Accel options are +/- 2, 4, 8, or 16 g
  imu.setAccelFSR(2); // Set accel to +/-2g

  // setLPF() can be used to set the digital low-pass filter
  // of the accelerometer and gyroscope.
  // Can be any of the following: 188, 98, 42, 20, 10, 5
  // (values are in Hz).
  imu.setLPF(5); // Set LPF corner frequency to 5Hz
  
// Use configureFifo to set which sensors should be stored
  // in the buffer.  
  // Parameter to this function can be: INV_XYZ_GYRO, 
  // INV_XYZ_ACCEL, INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
  imu.configureFifo(INV_XYZ_GYRO);

  imu.dmpBegin(DMP_FEATURE_GYRO_CAL |   // Enable gyro cal
              DMP_FEATURE_SEND_CAL_GYRO, // Send calibrated gyro values
              40);                   // Set DMP rate to 40 Hz

}


void updateGyro(){

    gyroX = imu.calcGyro(imu.gx);
    gyroY = imu.calcGyro(imu.gy);
    gyroZ = imu.calcGyro(imu.gz);

    // Also update the gyroSignal vector
    gyroSignal[0] = (double) gyroX;
    gyroSignal[1] = (double) gyroY;
    gyroSignal[2] = (double) gyroZ;

}

