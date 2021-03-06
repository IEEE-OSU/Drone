//--Arduino Drone receiver ppm decoder
// Author: Aaron Pycraft
// Date: 11/21/2016
// Synopsis: reads in digial signal from receiver and decodes it
// into ???
// References:
//  http://rcarduino.blogspot.com/2012/11/how-to-read-rc-receiver-ppm-stream.html


//--Constants
// receiver output signal 
const double FRAME = 1970; //us, length of each pulse
const double FRAME_SPACE = 1370; //us length of long low pulse

// to decode the ppm signal
int channelCounter = 0;

// related to electrical connections to arduino
const int CH1_INPUT_PIN = 2; // roll      (ch 1 on rx)
const int CH2_INPUT_PIN = 3; // pitch     (ch 2 on rx)
const int CH3_INPUT_PIN = 4; // throttle  (ch 3 on rx)
const int CH4_INPUT_PIN = 5; // yaw       (ch 4 on rx)

const int OUTPUT_1 = 6;

//--signal extrema
const float SIGNAL_MAX = 1890;
const float SIGNAL_MIN = 1040;
const float SIGNAL_RANGE = SIGNAL_MAX-SIGNAL_MIN;

int pulseLen = 0; //ms
int frame_space = 0;

void setup() {
  Serial.begin(115200);
  //--setup pin functions
  pinMode(CH1_INPUT_PIN, INPUT);
  pinMode(CH2_INPUT_PIN, INPUT);
  pinMode(CH3_INPUT_PIN, INPUT);
  pinMode(CH4_INPUT_PIN, INPUT);
  pinMode(OUTPUT_1, OUTPUT);  
}

void loop() {

  analogWrite(OUTPUT_1, readChannel(1));
  
//  displayChannelsSerial();
//    Serial.println(readSpaceLength());

}

//--Reads the width of all channels
void readChannels() {}



int readChannel(int channelNum) {
  int inputPin;
  switch(channelNum) {
    case 1: 
       inputPin = CH1_INPUT_PIN;
       break;
    case 2: 
       inputPin = CH2_INPUT_PIN;
       break;       
    case 3: 
       inputPin = CH3_INPUT_PIN;    
       break;     
    case 4: 
       inputPin = CH4_INPUT_PIN;    
       break;
//    case 5: 
//       inputPin = CH1_INPUT_PIN;
//       break;
//    case 6: 
//       inputPin = CH1_INPUT_PIN;
//       break;
//    case 7: 
//       inputPin = CH1_INPUT_PIN;
//       break;
//    case 8: 
//       inputPin = CH1_INPUT_PIN;
//       break;
    default:
      inputPin = CH1_INPUT_PIN;
      break;
  }

  // find pulse length from given pin
  int inputVal = pulseIn(inputPin, HIGH);
  // map input value to value between 0 and 255
  int returnVal = map(inputVal, SIGNAL_MIN, SIGNAL_MAX, 0, 255);

  //  Serial.println(inputVal);
  //  Serial.println(returnVal);
  return returnVal;
}

int readSpaceLength() {
  return pulseIn(2, LOW);
  
}

