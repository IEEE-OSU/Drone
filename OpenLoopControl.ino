// Possible example of software implementation for open-loop control

// -----   GLOBAL VARIABLES   ----- //

// For our sanity: Roll/theta = R, Pitch/phi = P, Yaw/psi = Y.

// Assign constants (placeholders of 1 used)
const float kT = 1; // kT found through experimental values
const float kI = 1; // kI = (3m + M)^-1 by theory
const float kY = 1; // kY can be assigned at will
// Note: All of the three must be strictly greater than (and excluding) zero (0).

// Define input variables to be passed
int inU = 0; int inR = 0; int inP = 0; int inY = 0;

// List the range of integer values present for the control
// signals when they are pulled from the receiver (placeholders)
int inLow = 0; int inHigh = 255;

// Solve the maximum range of the input-end mapping function
float Nmax = sqrt((1 / (4 * kT)) + (1 / (2 * kI*kT)) + (1 / (4 * kY)));

// List the range of integer values used to output speeds for the ESCs
// (again, placeholders)
int outLow = 0; int outHigh = 255;

// Define output variables for each motor
int M1 = 0; int M2 = 0; int M3 = 0; int M4 = 0; 


// -----   FUNCTIONS   ----- //

float mapIntToFloat(int x, int in_min, int in_max, float out_min, float out_max)
{
  // Cast the input limit integers to floats
  x = (float) x; in_min = (float) in_min; in_max = (float) in_max;
  // Perform the map, return the float
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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

int controlsToSpeeds(int inUvar, int inRvar, int inPvar, int inYvar, int motorID) {
  // Step 1: Map the input control variables to values for the vector formula
  float U = mapIntToFloat(inUvar, inLow, inHigh, 0, 1);
  float R = mapIntToFloat(inRvar, inLow, inHigh, -1, 1);
  float P = mapIntToFloat(inPvar, inLow, inHigh, -1, 1);
  float Y = mapIntToFloat(inYvar, inLow, inHigh, -1, 1);
  // Step 2: Put the variables into the proper vector equation for each motor
  float Nsquared = 0;
  switch (motorID) {
    case 1:
      Nsquared = (U / (4 * kT)) + (R / (2 * kI * kT)) + (Y / (4 * kY));
      break;
    case 2:
      Nsquared = (U / (4 * kT)) + (P / (2 * kI * kT)) - (Y / (4 * kY));
      break;
    case 3:
      Nsquared = (U / (4 * kT)) - (R / (2 * kI * kT)) + (Y / (4 * kY));
      break;
    case 4:
      Nsquared = (U / (4 * kT)) - (P / (2 * kI * kT)) - (Y / (4 * kY));
      break;
  }
  // Step 3: Enforce the zero-lock if negative and square-root the equation result.
  if (Nsquared < 0) {
    Nsquared = 0;
  }
  float N = sqrt(Nsquared);
  // Step 4: Map the speed command back to the integers of the motor. Return this.
  return mapFloatToInt(N, 0, Nmax, outLow, outHigh);
}

// -----   LOOPS   ----- //

void setup() {
  // put your setup code here, to run once:

  // The actual sketch will have stuff here.
}

void loop() {
  // put your main code here, to run repeatedly:

  /* Assume at this point, the values from reading the PPM have been assigned to the
      variables inU, inR, inP, inY. This is the part where the controlsToSpeeds function
      is called multiple times to gather the speeds for each motor.
  */
  M1 = controlsToSpeeds(inU, inR, inP, inY, 1);
  M2 = controlsToSpeeds(inU, inR, inP, inY, 2);
  M3 = controlsToSpeeds(inU, inR, inP, inY, 3);
  M4 = controlsToSpeeds(inU, inR, inP, inY, 4);
  /* Then after here, the four motor speeds would be placed in another function to
      the four ESCs.
  */

}
