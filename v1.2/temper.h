
enum
{
	STATE_NOP		=0,
	TEMP_ON			=2,
	TEMP1_START =6,
	MEAS_NOP1,
	TEMP1_MEAS,
	MEAS_NOP2,
	TEMP2_MEAS,
	MEAS_NOP3,
	V2_SENS0_MEAS,	//V2 block of sensors
	V2_SENS1_MEAS,
	V2_SENS2_MEAS,
	V2_SENS3_MEAS,
	V2_SENS4_MEAS,
	V2_SENS5_MEAS,
	V2_SENS6_MEAS,
	V2_SENS7_MEAS,
};

enum
{
  HEAT_NOP=0,
	HEAT_DO,
};
enum
{
  SHIM_MIN=0,
	SHIM_MAX=100,
};

#define INTEG_MAX  5110000l
#define INTEG_MIN -5110000l
#define TIMER_PID_MAX 10

#define ERR_T 100 // 100 = 1grad
#define ADC_CH_QNT 3

 #define KOEF_AMP 10.45f  // 25.175f 

extern void temp_meas(void);
extern void temp_control(void);
