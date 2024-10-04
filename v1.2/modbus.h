

#define ADR_DEVICE 1
//////////////////////////////// коды функций //////////////////////////////////
#define READ_COILS        0x01     // чтение логич. выхода
#define WRITE_COIL        0x05     // запись логич. выхода
#define WRITE_MULTI_COIL  0x0F     // запись мульти выхода сoil

#define READ_DISCR        0x02     // чтение дискретного входа
#define READ_HOLD         0x03     // чтение логич. выхода
#define READ_INPUT        0x04     // чтение аналог. входа
#define WRITE_SINGLE      0x06

#define WRITE_REGISTERS   0x10     // запись мульти выхода registers
////////////////////////////////////////////////////////////////////////////////
#define ERR_ILL_FUNC 0x01
#define ERR_ILL_REG  0x02
#define ERR_ILL_VAL  0x03
#define ERR_ILL_INIT 0x07
#define ERR_ILL_CRC  0x15
/////////////////////////////// flag_connect ///////////////////////////////////
#define GET_MSG 0x01
#define NO_MSG  0x02
#define NO2_MSG  0x04
////////////////////////////////////////////////////////////////////////////

extern void ModBus0_answers(void);
extern void ModBus1_answers(void);

