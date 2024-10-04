

#include "USART.h"
#include "Modbus.h"
#include "data.h"

extern unsigned int crc16(volatile unsigned char * array, unsigned char size);
extern void WriteFlash0R(void);

static volatile unsigned char mod_send[USART_SIZE_MSG];

/////////////////////////// ответ ќ–— серверу //////////////////////////////
extern void ModBus0_answers(void)
{
  unsigned char temp;
  unsigned int reg_addr;       /// адрес первого регистра
  unsigned int reg_qnt;        /// кол-во регистров
  //unsigned int reg_byte;       /// кол-во байт
  unsigned char byte_qnt=0;    /// счетчик отправл€емых/получаемых байт
  //unsigned int data_address;
  unsigned int value;
  unsigned char i; // счет регистров
	//static float f; // test
	//static unsigned char* ptr; // test
	//static unsigned int lsb, msb, val; // test
	//static unsigned long longdata;
	float fval;
  
  temp=USART0_flag_get();
    
  if(temp&USART_RECEIVE_END)
  { 	
		 temp&=~USART_RECEIVE_END;		
     USART0_flag_set(temp);	
				
     if(USART0_DataIn_get(0)==ADR_DEVICE)
     {
        if(!( crc16(USART0_PtrIn_get(),USART0_QntIn_get())) )
        {
           switch(USART0_DataIn_get(1))
           {
            /////////////////////////// HOLDING REGISTERS //////////////////////
            case READ_HOLD:	 //0x03 Read regs
                             reg_addr=(USART0_DataIn_get(2)<<8)|USART0_DataIn_get(3);
                             reg_qnt =(USART0_DataIn_get(4)<<8)|USART0_DataIn_get(5);
						
						
                             //////////////////// »ћя ћќƒ”Ћя ///////////////////
                             if(reg_addr==ADR_REG_NAME)
                             {
                                if(reg_qnt<=QNT_BYTE_NAME)
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt<<1);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    mod_send[(byte_qnt+3)]=(unsigned char)(registers_get((i+DATA_NAME1))>>8);
                                    mod_send[(byte_qnt+4)]=(unsigned char)registers_get((i+DATA_NAME1));
                                    byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char)crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5)); 
                                }
                                else
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                                  mod_send[2]=ERR_ILL_REG;
                                  mod_send[3]=(unsigned char)crc16(mod_send,3);
                                  mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                                  USART0_send(mod_send,5);
                                }
                             }
														 //////////////////// »ћя ћќƒ”Ћя ///////////////////
                             else if(reg_addr==ADR_REG_VERS)
                             {
                                if(reg_qnt<=QNT_BYTE_VER)
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt<<1);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    mod_send[(byte_qnt+3)]=(unsigned char)(registers_get((i+DATA_VER1))>>8);
                                    mod_send[(byte_qnt+4)]=(unsigned char)registers_get((i+DATA_VER1));
                                    byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char)crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5)); 
                                }
                                else
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                                  mod_send[2]=ERR_ILL_REG;
                                  mod_send[3]=(unsigned char)crc16(mod_send,3);
                                  mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                                  USART0_send(mod_send,5);
                                }
                             }
                             
														 //////////////////// RESISTERs ///////////////////
                             //03->030 Rval #1
														 else if(reg_addr==ADR_REG_RESIST)
                             {
                                if(reg_qnt<=QNT_REG_RESIST)
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt<<1);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    mod_send[(byte_qnt+3)]=(unsigned char)(registers_get((i+REG_R1_LSB))>>8);
                                    mod_send[(byte_qnt+4)]=(unsigned char)registers_get((i+REG_R1_LSB));
                                    byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char)crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5));

                                }
                                else
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                                  mod_send[2]=0x02;
                                  mod_send[3]=(unsigned char)crc16(mod_send,3);
                                  mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                                  USART0_send(mod_send,5);
                                }
                             }
														 //03->056 Rval #2
														 else if(reg_addr==ADR_REG_RESIST2)
                             {
                                if(reg_qnt<=QNT_REG_RESIST/2) //?
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt<<1);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    mod_send[(byte_qnt+3)]=(unsigned char)(registers_get((i+REG_R20_LSB))>>8);
                                    mod_send[(byte_qnt+4)]=(unsigned char)registers_get((i+REG_R20_LSB));
                                    byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char)crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5));

                                }
                                else
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                                  mod_send[2]=0x02;
                                  mod_send[3]=(unsigned char)crc16(mod_send,3);
                                  mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                                  USART0_send(mod_send,5);
                                }
                             }
                             //////////////////// MEAS ///////////////////
														 //03->001 Meas start/stop
                             else if(reg_addr==ADR_REG_MEAS)
                             {
                                if(reg_qnt<=QNT_REG_MEAS)
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt<<1);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    mod_send[(byte_qnt+3)]=(unsigned char)(registers_get((i+REG_MEAS_NUM))>>8);
                                    mod_send[(byte_qnt+4)]=(unsigned char)registers_get((i+REG_MEAS_NUM));
                                    byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char)crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5));

                                }
                                else
                                {
                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                                  mod_send[2]=0x02;
                                  mod_send[3]=(unsigned char)crc16(mod_send,3);
                                  mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                                  USART0_send(mod_send,5);
                                }
                             }
														 													 
														 ///////////// CMD //////////////
														 //03->300 Cmd
														 else if(reg_addr==ADR_REG_CMD)
                             {
                                if(reg_qnt<=QNT_REG_CMD)
                                {
																	mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt<<1);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    mod_send[(byte_qnt+3)]=(unsigned char)(registers_get((i+REG_MEAS_CMD))>>8);
                                    mod_send[(byte_qnt+4)]=(unsigned char)registers_get((i+REG_MEAS_CMD));
                                    byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char)crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5));

																}
														 }		
														 ////////////// TRG /////////////
														 //03->302 Hetr setts
                             else if(reg_addr==ADR_REG_HET)
                             {
                                if(reg_qnt<=QNT_REG_TRG)
                                {
																	mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt<<1);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    mod_send[(byte_qnt+3)]=(unsigned char)(registers_get((i+DATA_TMP_t_LSB))>>8);
                                    mod_send[(byte_qnt+4)]=(unsigned char) registers_get((i+DATA_TMP_t_LSB));
                                    byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char) crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5));	
                                 																															
																}
															}

													 
														 //V2 03->400
														 else if(reg_addr==V2_ADR_CmdAsReg)
                             {
                                if(reg_qnt<=QNT_V2_All)
                                {
																	mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt<<1);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    mod_send[(byte_qnt+3)]=(unsigned char) registers_get((V2_Cmd_START + i));
                                    mod_send[(byte_qnt+4)]=(unsigned char)(registers_get((V2_Cmd_START + i))>>8);
                                    byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char) crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5));																									
																}
														 }
														 //V2 03->410
														 else if(reg_addr==V2_ADR_RegWrite)
                             {
                                if(reg_qnt<=QNT_V2_RegW)
                                {
																	mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt*2);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    value = registers_get((V2_Heat_0R0_OHM + i));
																		mod_send[(byte_qnt+3)]=(unsigned char) (value>>8);
																		mod_send[(byte_qnt+4)]=(unsigned char) (value);
																		byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char) crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5));																									
																}
														 }
														 
														 //V2 03->420
														 else if(reg_addr==V2_ADR_RegRead)
                             {
                                if(reg_qnt<=QNT_V2_RegR)
                                {
																	mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=READ_HOLD;
                                  mod_send[2]=(reg_qnt*2);
                                  for(i=0;i<reg_qnt;i++)
                                  {
                                    value = registers_get((V2_HeartBeat + i));
																		mod_send[(byte_qnt+3)]=(unsigned char) (value>>8);
																		mod_send[(byte_qnt+4)]=(unsigned char) (value);
																		byte_qnt+=2;
                                  }
                                  mod_send[(byte_qnt+3)]=(unsigned char) crc16(mod_send,(byte_qnt+3));
                                  mod_send[(byte_qnt+4)]=(unsigned char)(crc16(mod_send,(byte_qnt+3))>>8);
                                  USART0_send(mod_send,(byte_qnt+5));																									
																}
														 }
														 
                             else
                             {
                                mod_send[0]=ADR_DEVICE;
                                mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                                mod_send[2]=ERR_ILL_REG;
                                mod_send[3]=(unsigned char)crc16(mod_send,3);
                                mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                                USART0_send(mod_send,5);
                             }
            break;
            
						//-----------------------------------------------------------------
						case WRITE_SINGLE: //0x06
                             reg_addr=(USART0_DataIn_get(2)<<8) | USART0_DataIn_get(3);
                             value   =(USART0_DataIn_get(4)<<8) | USART0_DataIn_get(5);
						
                             /////////////////////// јƒ–≈— /////////////////////
                             if(reg_addr==ADR_REG_NAME)
                             {
                                 registers_set(value,DATA_ADR);

                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=WRITE_SINGLE;
                                  mod_send[2]=USART0_DataIn_get(2);
                                  mod_send[3]=USART0_DataIn_get(3);
                                  mod_send[4]=USART0_DataIn_get(4);
                                  mod_send[5]=USART0_DataIn_get(5);
                                  mod_send[6]=(unsigned char)crc16(mod_send,6);
                                  mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
                                  USART0_send(mod_send,8);
                             }
														 
														 //V2 06->410, 0R0 OHM 
														 else if(reg_addr==V2_ADR_Heat_0R0_OHM)
                             {
                                 registers_set(value,V2_Heat_0R0_OHM);

                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=WRITE_SINGLE;
                                  mod_send[2]=USART0_DataIn_get(2);
                                  mod_send[3]=USART0_DataIn_get(3);
                                  mod_send[4]=USART0_DataIn_get(4);
                                  mod_send[5]=USART0_DataIn_get(5);
                                  mod_send[6]=(unsigned char)crc16(mod_send,6);
                                  mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
                                  USART0_send(mod_send,8);
																
																	registers_set(value,V2_Heat_0R0_OHM);
																	fval = value/10 + (value%10)/10.0;
																	value = *(unsigned int*)(&fval);															 
																	//sawp bytes for V1
																	registers_set( (((value & 0xFF000000)>>8) | ((value & 0x00FF0000)<<8))>>16,	DATA_TMP_R0_MSB);
																	registers_set( ((value & 0x000000FF)<<8) | ((value & 0x0000FF00)>>8), DATA_TMP_R0_LSB);
																	
																	registers_set(VAL_SET,DATA_UPT_MEMORY); //all upd
                             }
														 //V2 06->411, 0R1 OHM 
                             else if(reg_addr==V2_ADR_Heat_0R1_OHM)
                             {
                                 registers_set(value,V2_Heat_0R1_OHM);

                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=WRITE_SINGLE;
                                  mod_send[2]=USART0_DataIn_get(2);
                                  mod_send[3]=USART0_DataIn_get(3);
                                  mod_send[4]=USART0_DataIn_get(4);
                                  mod_send[5]=USART0_DataIn_get(5);
                                  mod_send[6]=(unsigned char)crc16(mod_send,6);
                                  mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
                                  USART0_send(mod_send,8);
															 
																	registers_set(VAL_SET,DATA_UPT_MEMORY);
                             }													 
														 //V2 06->412, Rotat
                             else if(reg_addr==V2_ADR_Rotat)
                             {
                                 registers_set(value,V2_Rotat);

                                  mod_send[0]=ADR_DEVICE;
                                  mod_send[1]=WRITE_SINGLE;
                                  mod_send[2]=USART0_DataIn_get(2);
                                  mod_send[3]=USART0_DataIn_get(3);
                                  mod_send[4]=USART0_DataIn_get(4);
                                  mod_send[5]=USART0_DataIn_get(5);
                                  mod_send[6]=(unsigned char)crc16(mod_send,6);
                                  mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
                                  USART0_send(mod_send,8);
                             }													 

                             else{
                                mod_send[0]=ADR_DEVICE;
                                mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                                mod_send[2]=ERR_ILL_REG;
                                mod_send[3]=(unsigned char)crc16(mod_send,3);
                                mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                                USART0_send(mod_send,5);
                             }
             break;
														 
						//-----------------------------------------------------------------
						//V2 05->400
						case WRITE_COIL:
							reg_addr=(USART0_DataIn_get(2)<<8)|USART0_DataIn_get(3);
							value   =(USART0_DataIn_get(4)<<8)|USART0_DataIn_get(5);
						
							if(reg_addr==V2_ADR_Cmd_START) {registers_set(value, V2_Cmd_START); registers_set(value, REG_MEAS_CMD);}
							if(reg_addr==V2_ADR_Cmd_HEAT)  {registers_set(value, V2_Cmd_HEAT);  registers_set(value, REG_MEAS_PRE);}
							if(reg_addr==V2_ADR_Cmd_Heat_Boost)  registers_set(value, V2_Cmd_Heat_Boost);
							if(reg_addr==V2_ADR_Cmd_Heat_Still)  registers_set(value, V2_Cmd_Heat_Still);
							if(reg_addr==V2_ADR_Cmd_Motor)  registers_set(value, V2_Cmd_Motor);
							if(reg_addr==V2_ADR_Cmd_Valve)  registers_set(value, V2_Cmd_Valve);
							if(reg_addr==V2_ADR_Cmd_Test)   registers_set(value, V2_Cmd_Test);
						
						mod_send[0]=ADR_DEVICE;
						mod_send[1]=WRITE_COIL;
						mod_send[2]=USART0_DataIn_get(2);
						mod_send[3]=USART0_DataIn_get(3);
						mod_send[4]=USART0_DataIn_get(4);
						mod_send[5]=USART0_DataIn_get(5);
						mod_send[6]=(unsigned char)crc16(mod_send,6);
						mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
						USART0_send(mod_send,8);

						break;
						
						
						//-----------------------------------------------------------------
						//W R I T E FUNC=0X10
					   case WRITE_REGISTERS: //0x10
							               reg_addr=(USART0_DataIn_get(2)<<8)|USART0_DataIn_get(3);
				                		 reg_qnt =(USART0_DataIn_get(4)<<8)|USART0_DataIn_get(5);
														 
															//Wr 300-301h
						                 if(reg_addr==ADR_REG_CMD)
                             {
															 byte_qnt=0;
															 for(i=0;i<reg_qnt;i++)
															 {
																	value=(USART0_DataIn_get((8+byte_qnt))<<8)|USART0_DataIn_get((7+byte_qnt));
																	registers_set(value,(i+REG_MEAS_CMD));
																 
																	byte_qnt+=2;
															 }
														 	 mod_send[0]=ADR_DEVICE;
															 mod_send[1]=WRITE_REGISTERS;
														 	 mod_send[2]=USART0_DataIn_get(2);
															 mod_send[3]=USART0_DataIn_get(3);
															 mod_send[4]=USART0_DataIn_get(4);
															 mod_send[5]=USART0_DataIn_get(5);
															 mod_send[6]=(unsigned char)crc16(mod_send,6);
															 mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
															 USART0_send(mod_send,8);
															 
																 //V2, map V1Heat->V2Heat
																registers_set( registers_get(REG_MEAS_PRE), V2_Cmd_HEAT);
													   }
														 
														 //Wr 302-30B
														 else if(reg_addr==ADR_REG_HET)
                             {
															 byte_qnt=0;
															 for(i=0;i<reg_qnt;i++)
															 {
																	value=(USART0_DataIn_get((8+byte_qnt))<<8)|USART0_DataIn_get((7+byte_qnt));
																	registers_set(value,(i+DATA_TMP_t_LSB));
																 
																	byte_qnt+=2;
															 }
															 
														 	 mod_send[0]=ADR_DEVICE;
															 mod_send[1]=WRITE_REGISTERS;
														 	 mod_send[2]=USART0_DataIn_get(2);
															 mod_send[3]=USART0_DataIn_get(3);
															 mod_send[4]=USART0_DataIn_get(4);
															 mod_send[5]=USART0_DataIn_get(5);
															 mod_send[6]=(unsigned char)crc16(mod_send,6);
															 mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
															 USART0_send(mod_send,8);
															 
												       registers_set(VAL_SET,DATA_UPT_MEMORY);	
													   }
														 
														 //Wr 30C-30F
														 else if(reg_addr==ADR_REG_PID)
                             {
															 byte_qnt=0;
															 for(i=0;i<reg_qnt;i++)
															 {
																	value=(USART0_DataIn_get((8+byte_qnt))<<8)|USART0_DataIn_get((7+byte_qnt));
																	registers_set(value,(i+DATA_TMP_Kp_LSB));
																 
																	byte_qnt+=2;
															 }															 
														 	 mod_send[0]=ADR_DEVICE;
															 mod_send[1]=WRITE_REGISTERS;
														 	 mod_send[2]=USART0_DataIn_get(2);
															 mod_send[3]=USART0_DataIn_get(3);
															 mod_send[4]=USART0_DataIn_get(4);
															 mod_send[5]=USART0_DataIn_get(5);
															 mod_send[6]=(unsigned char)crc16(mod_send,6);
															 mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
															 USART0_send(mod_send,8);
															 
												       registers_set(VAL_SET,DATA_UPT_MEMORY);	
													   }
														 
														 //Wr 310-312
													   else if(reg_addr==ADR_REG_MSK)
                             {
															 byte_qnt=0;
															 for(i=0;i<reg_qnt;i++)
															 {
																	value=(USART0_DataIn_get((8+byte_qnt))<<8)|USART0_DataIn_get((7+byte_qnt));
																	registers_set(value,(i+DATA_TMP_MSK1));
																 
																	byte_qnt+=2;
															 }															 
														 	 mod_send[0]=ADR_DEVICE;
															 mod_send[1]=WRITE_REGISTERS;
														 	 mod_send[2]=USART0_DataIn_get(2);
															 mod_send[3]=USART0_DataIn_get(3);
															 mod_send[4]=USART0_DataIn_get(4);
															 mod_send[5]=USART0_DataIn_get(5);
															 mod_send[6]=(unsigned char)crc16(mod_send,6);
															 mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
															 USART0_send(mod_send,8);
															 
												       registers_set(VAL_SET,DATA_UPT_MEMORY);	
													   }

													   //V2 Wr 400
														 else if(reg_addr==V2_ADR_CmdAsReg)
                             {
															 byte_qnt=0;
															 for(i=0;i<reg_qnt;i++)
															 {
																	value=(USART0_DataIn_get( (8+byte_qnt))<<8) | USART0_DataIn_get((7+byte_qnt) );
																	registers_set(value,(V2_Cmd_START + i));
																 
																	byte_qnt+=2;
															 }															 
														 	 mod_send[0]=ADR_DEVICE;
															 mod_send[1]=WRITE_REGISTERS;
														 	 mod_send[2]=USART0_DataIn_get(2);
															 mod_send[3]=USART0_DataIn_get(3);
															 mod_send[4]=USART0_DataIn_get(4);
															 mod_send[5]=USART0_DataIn_get(5);
															 mod_send[6]=(unsigned char)crc16(mod_send,6);
															 mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
															 USART0_send(mod_send,8);															 
													   }
														 
														 //V2 Wr 410
														 else if(reg_addr==V2_ADR_RegWrite)
                             {
															 byte_qnt=0;
															 for(i=0;i<reg_qnt;i++)
															 {
																	value=( USART0_DataIn_get(8+byte_qnt)) | (USART0_DataIn_get(7+byte_qnt) <<8 );
																	registers_set(value,(V2_Heat_0R0_OHM + i));
																 
																	byte_qnt+=2;
															 }															 
														 	 mod_send[0]=ADR_DEVICE;
															 mod_send[1]=WRITE_REGISTERS;
														 	 mod_send[2]=USART0_DataIn_get(2);
															 mod_send[3]=USART0_DataIn_get(3);
															 mod_send[4]=USART0_DataIn_get(4);
															 mod_send[5]=USART0_DataIn_get(5);
															 mod_send[6]=(unsigned char)crc16(mod_send,6);
															 mod_send[7]=(unsigned char)(crc16(mod_send,6)>>8);
															 USART0_send(mod_send,8);
													   }
														 
														 else
                             {
                                mod_send[0]=ADR_DEVICE;
                                mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                                mod_send[2]=ERR_ILL_REG;
                                mod_send[3]=(unsigned char)crc16(mod_send,3);
                                mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                                USART0_send(mod_send,5);
                             }
														break;
             
						 default:
                      mod_send[0]=ADR_DEVICE;
                      mod_send[1]=USART0_DataIn_get(1)|(1<<7);
                      mod_send[2]=0x01;
                      mod_send[3]=(unsigned char)crc16(mod_send,3);
                      mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
                      USART0_send(mod_send,5);
             break;
           }
        }
        else
        {
          mod_send[0]=ADR_DEVICE;
          mod_send[1]=(USART0_DataIn_get(1)|(1<<7));
          mod_send[2]=0x15;
          mod_send[3]=(unsigned char)crc16(mod_send,3);
          mod_send[4]=(unsigned char)(crc16(mod_send,3)>>8);
          USART0_send(mod_send,5);				 
        }
     }     	
  }
}



//V2 removed duu to perfomance, got lot of irq
//extern void ModBus1_answers(void)
