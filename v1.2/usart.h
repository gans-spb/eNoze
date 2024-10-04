
#define KBIT_9600 103                       // ������� 9600��� 
#define KBIT_19200 51                       // ������� 19200���
#define KBIT_38400 25                       // ������� 38400���
#define KBIT_230400 3                        // ������� 230400���
#define KBIT_115200 8                        // ������� 230400���
#define KBIT_1M 0                        // ������� 1M���

enum
{
	USART_MSG_BUSY=0x01,
	USART_RECEIVE_START=0x02,
	USART_RECEIVE_END=0x04,
};

// 
#define USART_SIZE_MSG 300        // 	a buffer size of input/output message

typedef struct
{
	volatile unsigned char count_in;       // ������� ������    
	volatile unsigned char count_out;      // ������� ��������    
	volatile unsigned char qnt_in;         // ���-�� �������� ����
	volatile unsigned char qnt_out;        // ���-�� ������������ ����
	volatile unsigned char a_input[USART_SIZE_MSG];
	volatile unsigned char a_out[USART_SIZE_MSG];
	volatile unsigned char flag;
} DataUsart_TypeDef;


extern unsigned char USART0_flag_get(void);
extern void USART0_flag_set(unsigned char value);
extern void USART0_send(volatile unsigned char *msg, unsigned int size);
extern unsigned char USART0_DataIn_get(unsigned int num);
extern unsigned int USART0_QntIn_get(void);
extern volatile unsigned char* USART0_PtrIn_get(void);
extern void USART0_EndReceive(void);

extern unsigned char USART1_flag_get(void);
extern void USART1_flag_set(unsigned char value);
extern void USART1_send(volatile unsigned char *msg, unsigned int size);
extern unsigned char USART1_DataIn_get(unsigned int num);
extern unsigned int USART1_QntIn_get(void);
extern volatile unsigned char* USART1_PtrIn_get(void);
extern void USART1_EndReceive(void);
