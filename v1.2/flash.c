
#include "gd32f10x_fmc.h"
#include "flash.h"
#include "data.h"
#include "float.h"

////////////////////////////////////////////////////////////////////////////////
//check if changes and write all data
void flash_update(void)
{
	unsigned int lsb, msb,value;
	unsigned int arr[QNT_FL_U32];
	static unsigned char count=0;
	
	if(registers_get(DATA_UPT_MEMORY)==VAL_SET)
	{
		++count;
		if(count>5) //delay
		{
			// t
			lsb=registers_get(DATA_TMP_t_LSB);
			msb=registers_get(DATA_TMP_t_MSB);
			value=lsb|(msb<<16);
			arr[ADR_FL_TMP_t]=value;
			// R0
			lsb=registers_get(DATA_TMP_R0_LSB);
			msb=registers_get(DATA_TMP_R0_MSB);
			value=lsb|(msb<<16);
			arr[ADR_FL_R0]=value;
			// A
			lsb=registers_get(DATA_TMP_A_LSB);
			msb=registers_get(DATA_TMP_A_MSB);
			value=lsb|(msb<<16);
			arr[ADR_FL_A]=value;
			// B
			lsb=registers_get(DATA_TMP_B_LSB);
			msb=registers_get(DATA_TMP_B_MSB);
			value=lsb|(msb<<16);
			arr[ADR_FL_B]=value;
			// Kp
			lsb=registers_get(DATA_TMP_Kp_LSB);
			msb=registers_get(DATA_TMP_Kp_MSB);
			value=lsb|(msb<<16);
			arr[ADR_FL_Kp]=value;
  	  // Ki
			lsb=registers_get(DATA_TMP_Ki_LSB);
			msb=registers_get(DATA_TMP_Ki_MSB);
			value=lsb|(msb<<16);
			arr[ADR_FL_Ki]=value;
			// MSK
			arr[ADR_FL_MSK1]=registers_get(DATA_TMP_MSK1);
			arr[ADR_FL_MSK2]=registers_get(DATA_TMP_MSK2);
			arr[ADR_FL_MSK3]=registers_get(DATA_TMP_MSK3);
			
			//V2 0R 0-1
			lsb=registers_get(V2_Heat_0R0_OHM);
			msb=registers_get(V2_Heat_0R1_OHM);
			value=lsb|(msb<<16);
			arr[V2_ADR_FL_0R]=value;
				
			flash_write_arr32(arr,QNT_FL_U32,ADR_FL_TMP_t,ADR_SITE_BASE);	
			
			registers_set(VAL_RESET,DATA_UPT_MEMORY);
	  }
  }
	else count=0;
}													 

////////////////////////////////////////////////////////////////////////////////
static unsigned int flash_read(unsigned int address){return (*(unsigned int*) address);}
////////////////////////////////////////////////////////////////////////////////
extern unsigned int flash_read_32(unsigned int adr_data,unsigned int adr_site)
{
	unsigned int address=(adr_site+adr_data*4);
  return (*(unsigned int*) address);
}
////////////////////////////////////////////////////////////////////////////////
extern void flash_write_16bit(unsigned short value, unsigned int adr_data,unsigned int adr_site)
{
	unsigned short i, copy[QNT_FL_U16];
	
	for(i=0;i<QNT_FL_U16;i++){copy[i]=flash_read((adr_site+(ADR_FL_TMP_t_LSB+i)*2));}
	copy[adr_data]=value;
	fmc_unlock();
	fmc_page_erase(adr_site); 
	for(i=0;i<QNT_FL_U16;i++) fmc_halfword_program((adr_site+(ADR_FL_TMP_t_LSB+i)*2),copy[i]);
	fmc_lock();
}
////////////////////////////////////////////////////////////////////////////////
extern void flash_write_32bit(unsigned int value, unsigned int adr_data,unsigned int adr_site)
{
	unsigned int i, copy[QNT_FL_U32];
	
	for(i=0;i<QNT_FL_U32;i++){copy[i]=flash_read((adr_site+(ADR_FL_TMP_t+i)*4));}
	copy[adr_data]=value;
	fmc_unlock();
	fmc_page_erase(adr_site); 
	for(i=0;i<QNT_FL_U32;i++) fmc_word_program((adr_site+(ADR_FL_TMP_t+i)*4),copy[i]);
	fmc_lock();
}
////////////////////////////////////////////////////////////////////////////////
extern void flash_write_arr32(unsigned int* value, unsigned int qnt, unsigned int adr_data,unsigned int adr_site)
{
	unsigned int i, copy[QNT_FL_U32];
	
	for(i=0;i<QNT_FL_U32;i++){copy[i]=flash_read((ADR_SITE_BASE+(ADR_FL_TMP_t+i)*4));}
	for(i=0;i<qnt;i++)copy[(adr_data+i)]=value[i];
	fmc_unlock();
	fmc_page_erase(adr_site); 
	for(i=0;i<QNT_FL_U32;i++) fmc_word_program((adr_site+(ADR_FL_TMP_t+i)*4),copy[i]);
	fmc_lock();
}
