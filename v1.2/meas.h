
#define HALF_PERIOD (FOSC/1000000)

#define Uref_mV 3300.0f     // mV
#define MIN_LVL_MEAS 500    // mV
#define MAX_LVL_MEAS 3000   // mV

#define CORRECT_LEVEL 7000000
#define CORRECT_COEF  1.06f

#define QNT_CHANNEL 39
#define VAL_ALARM 0xFFFF


enum
{
 MEAS_NEXT=0,
 MEAS_REPEAT
};
enum
{	
	MEAS_R0=0,
	MEAS_R1,
	MEAS_R2,
	MEAS_R3,
	MEAS_R4,
	MEAS_R5,
	MEAS_R6,
	MEAS_R7,
	MEAS_R8,
	MEAS_R9,
	MEAS_R10,
	MEAS_R11,
	MEAS_R12,
	MEAS_R13,
	MEAS_R14,
	MEAS_R15,
	MEAS_R16,
	MEAS_R17,
	MEAS_R18,
	MEAS_R19,
	MEAS_R20,
	MEAS_R21,
	MEAS_R22,
	MEAS_R23,
	MEAS_R24,
	MEAS_R25,
	MEAS_R26,
	MEAS_R27,
	MEAS_R28,
	MEAS_R29,
	MEAS_R30,
	MEAS_R31,
	MEAS_R32,
	MEAS_R33,
	MEAS_R34,
	MEAS_R35,
	MEAS_R36,
	MEAS_R37,
	MEAS_R38,
	MEAS_R39,
	
	MEAS_NOP,
	MEAS_Uref,
	MEAS_RESET_IO,
};

enum
{
  Rop_1k=1,
	Rop_10k,
	Rop_100k,
	Rop_1M,
	Rop_10M,
	Rop_50M,
};

enum
{
	REPEAT_1M=2,
  REPEAT_10M=5,//5,
	REPEAT_50M=7,//7,
	REPEAT_100M=10,//10,
	REPEAT_200M=15,//15,
};


extern void meas_cycle(void);
extern void meas_imitate(void);
extern void meas_init(void); 

