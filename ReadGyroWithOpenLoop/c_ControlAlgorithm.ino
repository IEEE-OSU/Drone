/* c_ControlAlgorithm.ino

    Author: Aaron Pycraft, Roger Kassouf
    Contributors: Roger Kassouf, Iwan Martin, Chen Liang, Aaron Pycraft

    Date last modified: 2017-03-04

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
const int kState3Midpoint = (kState1Max-kState1Min)/2; // 100

//--Motor transformation constants
const int kThrottleBound = (kState0Max - kState0Min)/4; // 200
const int kRotationsBound = (kThrottleBound/2)-1; // 100

//--Variables used in controlTransfer
  int T, R, P, Y;   // local copies of the 4 tx signals
  int M1, M2, M3, M4; // temp variables for motor speeds
  int NT, NP, NR, NY; // transformation factors

/* Inputs are txSignal, which enter this function in state 2 
 *  INPUT:  txSignal  = [R2 P2 Y2 T2], 2 denotes state 2 of values
 *  OUTPUT: motorsOut = [M1 M2 M3 M4]
 */
void controlTransfer(const unsigned int *txSignal, unsigned int *motorsOut) {
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
  NT = T;
  NP = constrain(P - 100,-NT/2,NT/2);
  NR = constrain(R - 100,-NT/2,NT/2);
  NY = constrain(Y - 100,-NT/2,NT/2);

    //DEBUG print N[T-P]
/*      Serial.print("Throttle: "); Serial.print(NT); Serial.print("\t");
  Serial.print("Roll: \t");   Serial.print(NR);     Serial.print("\t");
  Serial.print("Pitch: ");    Serial.print(NP);    Serial.print("\t");
  Serial.print("Yaw: \t");    Serial.print(NY);      Serial.print("\t");
  Serial.println();*/
    
    
    
    // Ensures that no commands are executed which would give invalid Servo numbers.
  // state 4 achieved


  //DEBUG set motors to throttle

  M1 = NT*5 + kServoMin;
  M2 = NT*5 + kServoMin;
  M3 = NT*5 + kServoMin;
  M4 = NT*5 + kServoMin;
  
  /*
  M1 = NT - NP + NY + kServoMin;
  M2 = NT - NR - NY + kServoMin;
  M3 = NT + NP + NY + kServoMin;
  M4 = NT + NR - NY + kServoMin;
  */
  
  /*
  M1 = NT + kServoMin;
  M2 = NT + kServoMin;
  M3 = NT + kServoMin;
  M4 = NT + kServoMin;
  */
  
  /* NOTE: This gives a potential value of any motor speed between 1200 and 1600.
  in Servo microseconds. We have calibrated the ESCs for 2000, which indicates a
  difference between 0% and 50% power at any given time. If one desired to use the
  full power range, they would simply multiply {M1, M2, M3, M4} by 2. */
    
  // Step 5: Re-map the speed values to each motor

  //DEBUG Print current motor output
  Serial.print("Motor Raw: ");
  Serial.println(NT*5);
  
  motorsOut[0] = M1;
  motorsOut[1] = M2;
  motorsOut[2] = M3;
  motorsOut[3] = M4; 
  
}
