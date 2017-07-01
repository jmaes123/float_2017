#include <SoftwareSerial.h>
#include <RoboClaw.h>
#include "bear.h"
#include "float_hardware.h"

//Robo Claw Velocity PID Coefficients
#define RC_Kp 1.0
#define RC_Ki 0.0
#define RC_Kd 0.0
#define RC_qpps 10700  //This is probably wrong.

//Set up a terminal for debug output.  Use hardware serial pins 0 and 1
SoftwareSerial terminal(0,1);

//Setup communcaitions with roboclaw. Use pins 10 and 11 with 10ms timeout
SoftwareSerial serial(10,11);  
RoboClaw roboclaw(&serial,10000);


unsigned long msTime = 0;
void setup() {
  //Open terminal and roboclaw serial ports
  terminal.begin(57600);
  roboclaw.begin(57600);
  
  //Set PID Coefficients
 roboclaw.SetM1VelocityPID(RCAddress,RC_Kd,RC_Kp,RC_Ki,RC_qpps);  

  pinMode(BEAR_CCW_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BEAR_CW_SWITCH_PIN, INPUT_PULLUP);
  pinMode(CONTROL_SWITCH_BEAR_PIN, INPUT_PULLUP);
  pinMode(CONTROL_SWITCH_MOOSE_PIN, INPUT_PULLUP);
  pinMode(CONTROL_SWITCH_COUGAR_PIN, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  delay(5000);
}




void loop() {
  static uint32_t lastEnc = 0;

     
   uint8_t status1,status2,status3,status4;
  bool valid1,valid2,valid3,valid4;
  
  stepBear();

}

