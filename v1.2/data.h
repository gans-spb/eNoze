

////////////////////////////////////////////////////////////////////////////////

#define QNT_REG_MEAS    18
#define QNT_REG_RESIST  96
#define QNT_BYTE_NAME   10
#define QNT_BYTE_VER    8
#define QNT_BYTE_ADR    6
#define QNT_REG_CMD     2
#define QNT_REG_TRG     17
#define QNT_REG_MSK     3
#define QNT_REG_IB      2
#define QNT_REG_V2      25

#define QNT_V2_RegR     24
#define QNT_V2_RegW     40 //((2+14) + QNT_V2_RegR)
#define QNT_V2_All      ((7+9)  + QNT_V2_RegW)

enum
{
  ADR_REG_NAME=0x0100,
  ADR_REG_VERS=0x0200,
  ADR_REG_RESIST=0x0030,
	ADR_REG_RESIST2=0x0056,
  ADR_REG_MEAS=0x0001,
	ADR_REG_CMD=0x0300,
	ADR_REG_HET=0x0302,
	ADR_REG_PID=0x030C,
	ADR_REG_MSK=0x0310,
	
	//V2 coil/reg addresses
		V2_ADR_CmdAsReg	 =0x0400,
		V2_ADR_RegWrite	 =0x0410,
		V2_ADR_RegRead	 =0x0420,
	
		V2_ADR_Cmd_START =0x0400,
		V2_ADR_Cmd_HEAT  =0x0401,
		V2_ADR_Cmd_Heat_Boost =0x0402,
		V2_ADR_Cmd_Heat_Still	=0x0403,
		V2_ADR_Cmd_Motor	=0x0404,
		V2_ADR_Cmd_Valve	=0x0405,
		V2_ADR_Cmd_Vent 	=0x0406,
		V2_ADR_Cmd_Test		=0x040F,
		
		V2_ADR_Heat_0R0_OHM = 0x0410,
		V2_ADR_Heat_0R1_OHM = 0x0411,
		V2_ADR_Rotat			  = 0x0412,
		
		V2_ADR_V2_HeartBeat = 0x0420
};	

///////////////////////////////// registers[] //////////////////////////////////
enum registers
{
    DATA_MODE=0,

    REG_R1_LSB,
    REG_R1_MSB,
    REG_R2_LSB,
    REG_R2_MSB,
    REG_R3_LSB,
    REG_R3_MSB,
    REG_R4_LSB,
    REG_R4_MSB,
    REG_R5_LSB,
    REG_R5_MSB,
    REG_R6_LSB,
    REG_R6_MSB,
    REG_R7_LSB,
    REG_R7_MSB,
    REG_R8_LSB,
    REG_R8_MSB,
    REG_R9_LSB,
    REG_R9_MSB,
    REG_R10_LSB,
    REG_R10_MSB,
    REG_R11_LSB,
    REG_R11_MSB,
    REG_R12_LSB,
    REG_R12_MSB,
    REG_R13_LSB,
    REG_R13_MSB,
    REG_R14_LSB,
    REG_R14_MSB,
    REG_R15_LSB,
    REG_R15_MSB,
    REG_R16_LSB,
    REG_R16_MSB,
    REG_R17_LSB,
    REG_R17_MSB,
    REG_R18_LSB,
    REG_R18_MSB,
    REG_R19_LSB,
    REG_R19_MSB,
    REG_R20_LSB,
    REG_R20_MSB,
    REG_R21_LSB,
    REG_R21_MSB,
    REG_R22_LSB,
    REG_R22_MSB,
    REG_R23_LSB,
    REG_R23_MSB,
    REG_R24_LSB,
    REG_R24_MSB,
    REG_R25_LSB,
    REG_R25_MSB,
    REG_R26_LSB,
    REG_R26_MSB,
    REG_R27_LSB,
    REG_R27_MSB,
    REG_R28_LSB,
    REG_R28_MSB,
    REG_R29_LSB,
    REG_R29_MSB,
    REG_R30_LSB,
    REG_R30_MSB,
    REG_R31_LSB,
    REG_R31_MSB,
    REG_R32_LSB,
    REG_R32_MSB,
    REG_R33_LSB,
    REG_R33_MSB,
    REG_R34_LSB,
    REG_R34_MSB,
    REG_R35_LSB,
    REG_R35_MSB,
    REG_R36_LSB,
    REG_R36_MSB,
    REG_R37_LSB,
    REG_R37_MSB,
    REG_R38_LSB,
    REG_R38_MSB,
		REG_MEAS_NUM_COPY,	//V2 Copy of 01 02 08 to get meas by one request
		REG_MEAS_CH_COPY,
		REG_MEAS_TIME_COPY,
		
    //
    REG_MEAS_CMD,
		REG_MEAS_PRE,
		//
		DATA_TMP_t_LSB,
		DATA_TMP_t_MSB,
		DATA_TMP_Rt_LSB,
		DATA_TMP_Rt_MSB,
		DATA_TMP_R0_LSB,
		DATA_TMP_R0_MSB,
		DATA_TMP_A_LSB,
		DATA_TMP_A_MSB,
		DATA_TMP_B_LSB,
		DATA_TMP_B_MSB,
		DATA_TMP_Kp_LSB,
		DATA_TMP_Kp_MSB,
		DATA_TMP_Ki_LSB,
		DATA_TMP_Ki_MSB,
		DATA_TMP_MSK1,   // Channel 1...16
		DATA_TMP_MSK2,   // Channel 17...32
		DATA_TMP_MSK3,   // Channel 33...38
    //
    REG_MEAS_NUM,
    REG_MEAS_TIME,
    REG_T1_GRAD,
    REG_T2_GRAD,
    REG_T1_OHM,
    REG_T2_OHM,
    REG_Uref,
		REG_MEAS_CH,
		//
		REG_RANGE_1_4,
		REG_RANGE_5_8,
		REG_RANGE_9_12,
		REG_RANGE_13_16,
		REG_RANGE_17_20,
		REG_RANGE_21_24,
		REG_RANGE_25_28,
		REG_RANGE_29_32,
		REG_RANGE_33_36,
		REG_RANGE_37_38,
		//
		DATA_SHIM_TRG,
		//
		DATA_UPT_MEMORY,
    //
    DATA_NAME1,
    DATA_NAME2,
    DATA_NAME3,
    DATA_NAME4,
    DATA_NAME5,
    DATA_NAME6,
    DATA_NAME7,
    DATA_NAME8,
		DATA_NAME9,
		DATA_NAME10,
    //
    DATA_VER1,
    DATA_VER2,
    DATA_VER3,
    DATA_VER4,
    DATA_SERIAL1,
    DATA_SERIAL2,
    DATA_SERIAL3,
    DATA_SERIAL4,
    
		//
    DATA_ADR,
    DATA_SPEED,
    DATA_PARITY,
    DATA_STOP,
    DATA_PROT,
		
		//V2-------------------------------
		//400-40F Write coil 05/10
		V2_Cmd_START,
		V2_Cmd_HEAT,
		V2_Cmd_Heat_Boost,
		V2_Cmd_Heat_Still,
		V2_Cmd_Motor,
		V2_Cmd_Valve,
		V2_Cmd_Reserve_6,
		V2_Cmd_Reserve_7,
		V2_Cmd_Reserve_8,
		V2_Cmd_Reserve_9,
		V2_Cmd_Reserve_A,
		V2_Cmd_Reserve_B,
		V2_Cmd_Reserve_C,
		V2_Cmd_Reserve_D,
		V2_Cmd_Reserve_E,
		V2_Cmd_Test,

		//410-41F Write reg 10
		V2_Heat_0R0_OHM,
		V2_Heat_0R1_OHM,
		V2_Rotat,
		V2_WReg_Reserve_3,
		V2_WReg_Reserve_4,
		V2_WReg_Reserve_5,
		V2_WReg_Reserve_6,
		V2_WReg_Reserve_7,
		V2_WReg_Reserve_8,
		V2_WReg_Reserve_9,
		V2_WReg_Reserve_A,
		V2_WReg_Reserve_B,
		V2_WReg_Reserve_C,
		V2_WReg_Reserve_D,
		V2_WReg_Reserve_E,
		V2_WReg_Reserve_F,
		
		//Read
		V2_HeartBeat,
		V2_Heat_R1,
		V2_Heat_R2,
		V2_Heat_T1,
		V2_Heat_T2,
		V2_Pcb_Temp,
		V2_1wSens1T,
		V2_1wSens1H,
		V2_1wSens2T,
		V2_1wSens2H,
		V2_1wSens3T,
		V2_1wSens3H,
		V2_1wSens4T,
		V2_1wSens4H,
		V2_1wSens5T,
		V2_1wSens5H,
		V2_I2C_Sens1_T,
		V2_I2C_Sens1_H,
		V2_I2C_Sens1_P,
		V2_I2C_Sens2_T,
		V2_I2C_Sens2_H,
		V2_I2C_Sens2_P,

   DATA_END
};
#define SIZE_REG DATA_END

////////////////////////////////////////////////////////////////////////////////
enum DATA_ADR_SLAVE
{
	ADR_DVC_INIT=0,
	ADR_DVC_MIN=1,
	ADR_DVC_MAX=40,
};
//
enum DATA_CMD
{
	CMD_START=0x01,
  CMD_STOP=0x00,	
};
//
///
enum DATA_RST_WRK
{
 RST_OFF=0,
 RST_ON=0x23,	
};


////////////////////////////////////////////////////////////////////////////////
#define USART_BUFF 50

#define READ_HOLD       0x03
#define WRITE_REGISTERS 0x10
 
#define USART_QNT_DVS    1
#define USART_ERROR_QTY  5

//
enum PORTB_IN
{
  BIT_IN_ALARM=1,
	BIT_GPIO_JS_RIGHT=12,
	BIT_GPIO_JS_LEFT=13,
	BIT_GPIO_JS_BACK=14,
	BIT_GPIO_JS_FORW=15,
	
	BIT_GPIO_JS_UP=14,
	BIT_GPIO_JS_DW=15,
};

#define VAL_SET   0xff
#define VAL_RESET 0x00
#define VAL_ALARM 0xFFFF
#define VAL_R_MAX 2500 // 25.0 Ohm 
#define VAL_R_MIN_DLT 0.2f

////////////////////////////////////////////////////////////////////////////////
extern void service_868M_set(unsigned char value, unsigned char num);
extern unsigned char service_868M_get(unsigned char num);

extern void registers_set(unsigned int value, unsigned int num);
extern unsigned int registers_get(unsigned int num);

extern volatile unsigned long* ADC_ptr_out(void);
extern unsigned int ADC_data_out(unsigned char num);

extern unsigned int change_bit(unsigned int data,unsigned char bit, unsigned char state);
