
#define FOSC 36000000
#define PRESC1 1024

#define TIME_5MS  	5*(FOSC/PRESC1)/1000
#define TIME_10MS  10*(FOSC/PRESC1)/1000

#define RTOS_TIME 			TIME_5MS
#define RTOS_TIME_STEP 	5

#define RTOS_TIME_0			0
#define RTOS_TIME_25MS  25/RTOS_TIME_STEP
#define RTOS_TIME_50MS  50/RTOS_TIME_STEP
#define RTOS_TIME_75MS  75/RTOS_TIME_STEP
#define RTOS_TIME_0S1   100/RTOS_TIME_STEP
#define RTOS_TIME_0S15  150/RTOS_TIME_STEP
#define RTOS_TIME_0S2   200/RTOS_TIME_STEP
#define RTOS_TIME_0S25  250/RTOS_TIME_STEP
#define RTOS_TIME_0S3   300/RTOS_TIME_STEP
#define RTOS_TIME_0S5   500/RTOS_TIME_STEP
#define RTOS_TIME_1S    1000/RTOS_TIME_STEP
#define RTOS_TIME_2S    2000/RTOS_TIME_STEP
#define RTOS_TIME_10S   10000/RTOS_TIME_STEP

extern void TIM1_Start(void);
extern void TIM2_Start(void);
extern void GD32_DelayTim2(unsigned short time);
	
extern unsigned char get_time_sys(unsigned int time);
extern unsigned char get_time0(unsigned int time);
extern unsigned char get_time1(unsigned int time);
