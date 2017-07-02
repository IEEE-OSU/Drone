/* e_ClosedLoopFeedback.ino

    Authors: Roger Kassouf, Iwan Martin

    Date last modified: 2017-06-26

    Objective: Using IMU feedback and commands from the transmitter,
    compute a new control input using the PIDLibary library

    INPUTS: 
      tx signals: [ROLL, PITCH, THROTTLE, YAW]
      IMU signals: Gryoscope in X, Y, and Z (deg/s or dps)
    
    OUTPUTS: An update input signal for the open-loop ControlAlgorithm
    
    Notes: Functions run inside the main tab will redirect to here. This
           intends to "close the loop" so that the original control
           algorithm may run as it did before.
   
  _   _   _  __          __     _____  _   _ _____ _   _  _____   _   _   _ 
 | | | | | | \ \        / /\   |  __ \| \ | |_   _| \ | |/ ____| | | | | | |
 | | | | | |  \ \  /\  / /  \  | |__) |  \| | | | |  \| | |  __  | | | | | |
 | | | | | |   \ \/  \/ / /\ \ |  _  /| . ` | | | | . ` | | |_ | | | | | | |
 |_| |_| |_|    \  /\  / ____ \| | \ \| |\  |_| |_| |\  | |__| | |_| |_| |_|
 (_) (_) (_)     \/  \/_/    \_\_|  \_\_| \_|_____|_| \_|\_____| (_) (_) (_)
                                                                            
                                                                            
   Closed loop feedback may result in unexpected motion behavior due to
   potentially unstable feedback mechanisms. Please ensure that there is
   a way to kill power if necessary. Before changning control loop gains,
   please test first using the Simulink model to ensure stability. */


   // So uh... Nothing here worked. Moved it to main tab.




