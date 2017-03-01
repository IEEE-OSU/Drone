/* b_MotorControl.ino

    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft

    Date last modified: February 26, 2017

    Objective: sketch contains functions that convert tx signals into motor
               control signals. 

    INPUTS: tx signals: [Roll, pitch, throttle, yaw]
    OUTPUTS: motor output values [motor1, motor2, motor3, motor4]
*/

// ~A7. Output pins and Servo objects
/* NOTES: Every motor will be given a Servo object correlated to some output pin.

 *  ** IMPORTANT ** Ensure that the pairs of motors (1,3) and (2,4) have these
    properties on the physical craft:
    1. That (1,3) are set to be the same directional rotation, as are (2,4)
    2. That the pairs (1,3) and (2,4) rotate in different directions
    3. That the motors (1,3) are set across from each other on the drone's cross,
       as are (2,4).
    (The established positive direction for the motors is counter-clockwise, and
    originally set for 1 and 3).
*/
Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;


// output pins must be PWM enabled pins.
#ifndef MOTOR_PINS
  #define MOTOR_PINS
  const unsigned int motor1pin = 5;
  const unsigned int motor2pin = 6;
  const unsigned int motor3pin = 7;
  const unsigned int motor4pin = 8;
#endif

// ~A8. Servo limits
/* NOTES: When calibrating the electronic speed controllers (ESCs), it was found
    that an ESC will accept a servo value ranging from 20 to 180. If it is
    possible somehow to increase this, that can be changed here.
*/

const unsigned int kServoMin = 20;
const unsigned int kServoMax = 180;

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
  motor1.write(kServoMin);
  motor2.write(kServoMin);
  motor3.write(kServoMin);
  motor4.write(kServoMin);
}

void attachAllMotors() {
  motor1.attach(motor1pin);
  motor2.attach(motor2pin);
  motor3.attach(motor3pin);
  motor4.attach(motor4pin);
}

// ~C3.3 powerMotors

void powerMotors(unsigned int *motorsOut) {
  motor1.write(motorsOut[0]);
  motor2.write(motorsOut[1]);
  motor3.write(motorsOut[2]);
  motor4.write(motorsOut[3]);
}

