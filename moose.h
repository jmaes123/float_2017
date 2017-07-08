#ifndef _MOOSE_H_
#define _MOOSE_H_

#define MOOSE_DUTY_CYCLE (32767)
#define MOOSE_ON_TIME 13000
#define MOOSE_OFF_TIME_FWD 3000
#define MOOSE_OFF_TIME_BKWD 3000

typedef enum {
  MS_OFF,
  MS_READY,
  MS_FORWARD,
  MS_FORWARD_PAUSE,
  MS_BACKWARD,
  MS_BACKWARD_PAUSE,
  MS_ERROR
} tMOOSE_STATES;

void stepMoose(void);
void stopMoose(void);
void turnMooseOff(void);
void turnMooseOn(void);

#endif 

