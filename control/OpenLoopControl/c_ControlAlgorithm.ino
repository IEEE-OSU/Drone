/* c_ControlAlgorithm.ino

    Author: Aaron Pycraft, Roger Kassouf
    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft

    Date last modified: 

    Objective: sketch contains functions that convert tx signals into motor
               control signals. 

    INPUTS: tx signals: [Roll, pitch, throttle, yaw]
    OUTPUTS: motor output values [motor1, motor2, motor3, motor4]
    Notes: controlTransfer function below follows closely from the doc
          "Design of a discrete open loop control algorithm.docx"
          
          The control algorithm is written as a series of operations on 
          variables as they transistion from state 0 (raw tx time signals)
          to the final state
*/

//--Constants used throughout algorithm
// kState1Min, kState1Max; need not be declared b/c of forced compilation order
const int kState3resolution = kState1Max - kState1Min;
const int kState3Midpoint = kState1Max/2;

//--Motor transformation constants
const unsigned int kThrottleBound = (kState0Max - kState0Min)/4;
const unsigned int kRotationsBound = (kThrottleBound/2)-1;
const int kNI = 25;
const int kNY = 5;
const int kI = kRotationsBound/(kNI);
const int kY = kRotationsBound/(kNY);
const int Ndiff = (kServoMax-kServoMin)-kNI-kNY;
const int TCut = (kThrottleBound*(kNI+kNY)/Ndiff) + 1;

//--Variables used in controlTransfer
  int T, R, P, Y;   // local copies of the 4 tx signals
  int M1, M2, M3, M4; // temp variables for motor speeds
  int NT, NP, NR, NY; // transformation factors

/* Inputs are txSignal, which enter this function in state 2 
 *  INPUT:  txSignal  = [R2 P2 Y2 T2], 2 denotes state 2 of values
 *  OUTPUT: motorsOut = [M1 M2 M3 M4]
 */
void controlTransfer(const unsigned int *txSignal, unsigned int *motorsOut) {
  //--For debugging, simply make all motors behave the same way
  motorsOut[0] = map(txSignal[THROTTLE], kState1Min, kState1Max, kServoMin, kServoMax);
  motorsOut[1] = map(txSignal[THROTTLE], kState1Min, kState1Max, kServoMin, kServoMax);
  motorsOut[2] = map(txSignal[THROTTLE], kState1Min, kState1Max, kServoMin, kServoMax);
  motorsOut[3] = map(txSignal[THROTTLE], kState1Min, kState1Max, kServoMin, kServoMax);
  /*
  
  // abandon array in favor of more readable operations on the signals
  // the output will be contained in motorsOut anyways
  // probably a bad idea to read/write the same place VERY FAST
  T = txSignal[THROTTLE];
  R = txSignal[ROLL];
  P = txSignal[PITCH];
  Y = txSignal[YAW];

  M1 = kServoMin;
  M2 = kServoMin;
  M3 = kServoMin;
  M4 = kServoMin;
  
  // Transform to state 4 (skip 3); map values from 0 to state 3 resolution
  // to get lower bound of txSignal to be 0 to $STATE3_MAX_VALUE
  T = T - kState1Min;
  R = R - kState1Min - kState3Midpoint;
  P = P - kState1Min - kState3Midpoint;
  Y = Y - kState1Min - kState3Midpoint;
  // state 4 achieved

  NT = Ndiff*T/kThrottleBound;
  NP = P/kI;
  NR = R/kI;
  NY = Y/kY;
  
  if (T < TCut) {
    M1 = NT + kServoMin;
    M2 = NT + kServoMin;
    M3 = NT + kServoMin;
    M4 = NT + kServoMin;
  }
  else {
    M1 = NT + NP + NY + kServoMin;
    M2 = NT + NR - NY + kServoMin;
    M3 = NT - NP + NY + kServoMin;
    M4 = NT - NR - NY + kServoMin;
  }

  // Step 5: Re-map the speed values to each motor
  motorsOut[0] = M1;
  motorsOut[1] = M2;
  motorsOut[2] = M3;
  motorsOut[3] = M4;
  */

}

