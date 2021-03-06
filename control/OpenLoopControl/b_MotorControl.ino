/* b_MotorControl.ino

    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft

    Date last modified: February 26, 2017

    Objective: sketch contains functions that convert tx signals into motor
               control signals. 

    INPUTS: tx signals: [Roll, pitch, throttle, yaw]
    OUTPUTS: motor output values [motor1, motor2, motor3, motor4]
*/

/*  Motor configuration information
 *  Ensure that the pairs of motors (1,3) and (2,4) have these
    properties on the physical craft:
    1. That (1,3) are set to be the same directional rotation, as are (2,4)
    2. That the pairs (1,3) and (2,4) rotate in different directions (CW vs CCW)
    3. That the motors (1,3) are set across from each other on the drone's cross,
       as are (2,4).
       (The established positive direction for the motors is counter-clockwise, and
       originally set for 1 and 3).
    4. The quad shape is an X, with motors setup as follows:
            
             FRONT                
               M1
               |
               |                              |
       M2------+------M4
               |
               |
               M3               
       
      
         M1         M4    
           \       /      M1 is "upper left corner of X"
             \   /        M2 is below M1
               X          M3 is right of M2
             /   \        M4 is above M3 and left of M1
           /       \
         M2         M3 
       
    5. Roll  is rotation about M1-M3 axis line
          (+) is when M2 is raised
       pitch is rotation about M2-M4 axis line
          (+) is when M1 is raised
       yaw is rotation about the plane that contains all 4 motors
*/
Servo ESC1;//motor1;
Servo ESC2;//motor2;
Servo ESC3;//motor3;
Servo ESC4;//motor4;

// ~A8. Servo limits
/* NOTES: When calibrating the electronic speed controllers (ESCs), it was found
    that an ESC will accept a servo value ranging from 20 to 180. If it is
    possible somehow to increase this, that can be changed here.
*/
// Note: continous servos can be written values between 1000 and 2000
//  where 1000 = fully CCW; (NEEDS TO BE EXPERIMENTALLY VERIFIED)
//  and   2000 = fully CW (speeds) (usually is the max speed)
const unsigned int kServoMin = 1200;//20;
const unsigned int kServoMax = 2000; // MAX is 2000, safe is 1500

// Motors have imbalances; we will use different servo ranges to compensate. 
// we want to make sure motors speeds between each motor is the same for a
// given servo output value.
const int kESC1min = kServoMin + 0;
const int kESC2min = kServoMin + 0;
const int kESC3min = kServoMin + 0;
const int kESC4min = kServoMin + 0;
// maximum values for each motor
const int kESC1max = kServoMax - 0;
const int kESC2max = kServoMax + 0;
const int kESC3max = kServoMax + 0;
const int kESC4max = kServoMax + 0;

/*
    ===============================================================================
    ~B. STATE VARIABLES
    ===============================================================================
    These variables are subject to change, as they are used incrementally
    throughout the sketch.
*/


// setup code for motor controls
void initMotorControl() {
  attachAllMotors();
  ESC1.writeMicroseconds(kESC1min);
  ESC2.writeMicroseconds(kESC2min);
  ESC3.writeMicroseconds(kESC3min);
  ESC4.writeMicroseconds(kESC4min);
}

void attachAllMotors() {
  ESC1.attach(ESC1pin, kESC1min, kESC1max);
  ESC2.attach(ESC2pin, kESC2min, kESC2max);
  ESC3.attach(ESC3pin, kESC3min, kESC3max);
  ESC4.attach(ESC4pin, kESC4min, kESC4max);
}

void disconnectAllMotors() {
  ESC1.detach();
  ESC2.detach();
  ESC3.detach();
  ESC4.detach();
}

// ~C3.3 powerMotors

void powerMotors(unsigned int *motorsOut) {
  ESC1.writeMicroseconds(motorsOut[0]);
  ESC2.writeMicroseconds(motorsOut[1]);
  ESC3.writeMicroseconds(motorsOut[2]);
  ESC4.writeMicroseconds(motorsOut[3]);
}

void setMotorsToMin() {
  ESC1.writeMicroseconds(kESC1min);
  ESC2.writeMicroseconds(kESC2min);
  ESC3.writeMicroseconds(kESC3min);
  ESC4.writeMicroseconds(kESC4min);
}

