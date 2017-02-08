/* a_DecodeTransmitterInput.ino

    Author: Roger Kassouf
    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft
    Date last modified: February 7, 2017

    Objective: 

    INPUTS: 
    OUTPUTS: 
*/
// elsewhere declared variables
extern unsigned int scaledInRollValue, scaledInPitchValue;
extern unsigned int scaledInYawValue, scaledInThrottleValue;

// ~A5. Dead zone constant
/* NOTES: This value +- the scaledInAvg will define the dead zone range. Inputs
    which fall under the range will revert back to scaledInAvg.
*/
const unsigned int deadZoneConstant = 3;


// ~A6. Control constants
/* NOTES: Each constant corresponds to different values inside of the control
    transfer matrix (CTM). Scroll through to see their implementation.

    Descriptions:

    ** PARAMETERS TO SELECT **
    speedWidth -- the absolute maximium difference in speed between two motors.
      It must be between 0 and 1, so something like 0.20.
    tiltYaw -- the ratio of speeds between tilting (roll and/or pitch) and the
      yaw. Reasonably, you would want this greater than 1, but for sure greater
      than 0. There is no true upper limit.

    ** PARAMETERS CALCULATED **
    kT -- Apportions part of the Throttle command to the motors.
    kI -- Apportions part of the Roll and Pitch commands to the motors.
    kY -- Apportions part of the Yaw command to the motors.
    Tcut -- The throttle speed at which the control transfer will switch on the
      rotational control. When T is less than Tcut, all motors will have the same
      speed.

    For a better description of each parameter, look in the controlTransfer function
*/
const float speedWidth = 0.2;
const float tiltYaw = 2;
const float kY = speedWidth / (1 + (tiltYaw / 2));
const float kI = (tiltYaw / 2) * kY;
const float kT = 4 - (2 * kI) - kY;
const float TCut = 2 * speedWidth / kT;

//const float Nmax = sqrt((1 / (4 * kT)) + (1 / (2 * kI*kT)) + (1 / (4 * kY)));
//const float Nmaxsquared = (1 / (4 * kT)) + (1 / (2 * kI*kT)) + (1 / (4 * kY));


/*
    ===============================================================================
    ~C. FUNCTIONS
    ===============================================================================
    All of the functions outside of setup and loop that make the sketch function!
*/

// ~C3.1 scaleValues
void scaleValues() {
  scaledInRollValue = deadZone(constrain(rawInRollValue / 4, scaledInMin, scaledInMax), deadZoneConstant);
  scaledInPitchValue = deadZone(constrain(rawInPitchValue / 4, scaledInMin, scaledInMax), deadZoneConstant);
  scaledInThrottleValue = constrain(rawInThrottleValue / 4, scaledInMin, scaledInMax);
  scaledInYawValue = deadZone(constrain(rawInYawValue / 4, scaledInMin, scaledInMax), deadZoneConstant);
}


// ~C2. Scaling and Mapping Functions
/* NOTES: The purpose of these functions is to assist in computing the scaled
    input values, after the raw input values have been read.
*/

// ~C2.1 deadZone
unsigned int deadZone(unsigned int scaledInValue, unsigned int deadZoneConstant) {
  if (abs(scaledInValue - scaledInAvg) <= deadZoneConstant) {
    scaledInValue = scaledInAvg;
  }
  return scaledInValue;
}

// ~C2.2 mapIntToFloat
float mapIntToFloat(int x, int in_min, int in_max, float out_min, float out_max)
{
  // Cast the input limit integers to floats
  x = (float) x; in_min = (float) in_min; in_max = (float) in_max;
  // Perform the map, return the float
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ~C2.3 mapFloatToInt
int mapFloatToInt(float x, float in_min, float in_max, int out_min, int out_max)
{
  // We will perform the math in floats first. Cast output limit integers to floats.
  out_min = (float) out_min; out_max = (float) out_max;
  // Perform the map (using floats!)
  float x_out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  // Cast the output variable as an integer. While this does perform an inherent
  // "rounding down" it should be reasonably close to the desired value.
  x_out = (int) x_out;
  // Return the integer
  return x_out;
}

// ~C3 Meta-functions
/* NOTES: Meta-functions use other functions to accomplish a broader goal.
*/



// ~C3.2 controlTransfer
void controlTransfer(unsigned int scaledInRoll, unsigned int scaledInPitch, unsigned int scaledInThrottle, unsigned int scaledInYaw) {
  // Step 1: Convert the scaledIn values to values on a signed/unsigned binary basis:
  float R = mapIntToFloat(scaledInRoll, scaledInMin, scaledInMax, -1, 1);
  float P = mapIntToFloat(scaledInPitch, scaledInMin, scaledInMax, -1, 1);
  float U = mapIntToFloat(scaledInThrottle, scaledInMin, scaledInMax, 0, 1);
  float Y = mapIntToFloat(scaledInYaw, scaledInMin, scaledInMax, -1, 1);

  // Step 2: Perform the CTM calculations for each motor
  float N1 = 0;
  float N2 = 0;
  float N3 = 0;
  float N4 = 0;
  float NT = (kT * U / 4);
  float NR = (kI * R / 2);
  float NP = (kT * P / 2);
  float NY = (kY * Y / 4);
  if (U <= TCut) {
    N1 = NT;
    N2 = NT;
    N3 = NT;
    N4 = NT;
  }
  else {
    N1 = NT + NR + NY;
    N2 = NT + NP - NY;
    N3 = NT - NR + NY;
    N4 = NT - NP - NY;
  }

  // Step 3: Constrain the squared speeds to [0, (Nmax)^2]
  N1 = constrain(N1, 0, 1);
  N2 = constrain(N2, 0, 1);
  N3 = constrain(N3, 0, 1);
  N4 = constrain(N4, 0, 1);
  // Step 5: Re-map the speed values to each motor
  motorsOut[0] = mapFloatToInt(N1, 0, 1, servoMin, servoMax);
  motorsOut[1] = mapFloatToInt(N2, 0, 1, servoMin, servoMax);
  motorsOut[2] = mapFloatToInt(N3, 0, 1, servoMin, servoMax);
  motorsOut[3] = mapFloatToInt(N4, 0, 1, servoMin, servoMax);
}
