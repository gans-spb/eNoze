

#define ADR_DEVICE 1
//////////////////////////////// ���� ������� //////////////////////////////////
#define READ_COILS        0x01     // ������ �����. ������
#define WRITE_COIL        0x05     // ������ �����. ������
#define WRITE_MULTI_COIL  0x0F     // ������ ������ ������ �oil

#define READ_DISCR        0x02     // ������ ����������� �����
#define READ_HOLD         0x03     // ������ �����. ������
#define READ_INPUT        0x04     // ������ ������. �����
#define WRITE_SINGLE      0x06

#define WRITE_REGISTERS   0x10     // ������ ������ ������ registers
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

