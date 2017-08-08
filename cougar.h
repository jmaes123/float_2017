#ifndef _COUGAR_H_
#define _COUGAR_H_


#define COUGAR_DUTY_CYCLE (32767)
#define COUGAR_PID_OUTPUT_MAX (((double)COUGAR_DUTY_CYCLE)/1000.0f)



#define COUGAR_ON_TIME 13000
#define COUGAR_OFF_TIME_FWD 1000
#define COUGAR_OFF_TIME_BKWD 1000

#define COUGAR_TRANSIT_TIME_MSEC 3500
#define COUGAR_TRANSIT_STEP_PERIOD_MSEC 100


#define COUGAR_Kp (1.3f)
#define COUGAR_Ki (0.07f)
#define COUGAR_Kd (0.0f)


#define COUGAR_DEADBAND (0.10f)
#define COUGAR_POT_VOLTAGE_RANGE (-2.5f)
//#define COUGAR_POT_VOLTAGE_MAX (2.7f)
//#define COUGAR_POT_VOLTAGE_MIN (4.9f)

#define COUGAR_POT_VOLTAGE_MAX (4.55f)
#define COUGAR_POT_VOLTAGE_MIN (2.6f)

typedef enum {
  CS_OFF,
  CS_READY,
  CS_FORWARD,
  CS_FORWARD_PAUSE,
  CS_BACKWARD,
  CS_BACKWARD_PAUSE,
  CS_ERROR
} tCOUGAR_STATES;

void initializeCougar(void);
void stepCougar(void);
void stopCougar(void);
void turnCougarOff(void);
void turnCougarOn(void);

#endif
