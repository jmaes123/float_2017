#ifndef _FLOAT_HARDWARE_H_
#define _FLOAT_HARDWARE_H_


#define BEAR_CCW_SWITCH_PIN 26 //Pin for the bear's counter-clockwise rotation limit switch
#define BEAR_CW_SWITCH_PIN 22 //Pin for the bear's clockwise rotation limit switch

#define CONTROL_SWITCH_BEAR_PIN 23
#define CONTROL_SWITCH_MOOSE_PIN 24
#define CONTROL_SWITCH_COUGAR_PIN 25


#define CONTROL_UPDOWN_MOOSE_A_PIN 30
#define CONTROL_UPDOWN_MOOSE_B_PIN 31

#define RCAddress 0x80 //Robo Claw Motor Controller's address


bool limitReached(int switchPin);
bool switchOn(int switchPin);


#endif

