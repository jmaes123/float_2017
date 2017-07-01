#ifndef _BEAR_H_
#define _BEAR_H_

#define BEAR_PAUSE_TIME_MSEC 1000
#define BEAR_ERROR_TIME_MSEC 15000
#define BEAR_DUTY_CYCLE (32767/3)
//32767

typedef enum {
  BS_OFF,
  BS_STOPPED,
  BS_ROTATING_CW,
  BS_PAUSED_CW, //The pause just after a clockwise rotation
  BS_ROTATING_CCW,
  BS_PAUSED_CCW, //The pause just after a counter-clockwise rotation
  BS_ERROR
} tBEAR_STATES;

void initBear(void);

void stopBear(void);
void stepBear(void);

#endif

