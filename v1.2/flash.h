

//extern unsigned short flash_read_16bit(unsigned int FlashAddr);
extern unsigned int flash_read_32(unsigned int adr_data,unsigned int adr_site);
	
extern void flash_write_16bit(unsigned short value, unsigned int adr_data,unsigned int adr_site);
extern void flash_write_32bit(unsigned int value, unsigned int adr_data,unsigned int adr_site);
extern void flash_write_arr32(unsigned int* value, unsigned int qnt, unsigned int adr_data,unsigned int adr_site);

//GD32F103xx memory map, 0x0800 0000 - 0x082F FFFF Main Flash
#define ADR_SITE_BASE		 0x0801E000   // 801E000=120страница,

enum FlashAddr
{
	ADR_FL_TMP_t_LSB=0,
  ADR_FL_TMP_t_MSB,
	
	ADR_FL_END
};
#define QNT_FL_U16 ADR_FL_END

enum FlashAddr_32
{
	ADR_FL_TMP_t=0,
  ADR_FL_R0,
	ADR_FL_A,
	ADR_FL_B,
	ADR_FL_Kp,
	ADR_FL_Ki,
	ADR_FL_MSK1,
	ADR_FL_MSK2,
	ADR_FL_MSK3,
	V2_ADR_FL_0R,  //V2 0R values
	ADR_FL_END_32
};


#define QNT_FL_U32 ADR_FL_END_32
