# eNose V2 Graphene project
# Igor Bocharov May-2024 Ioffe inst.
# eNose PCB V2 pymodbus client

import array
import msvcrt
import time
import struct

# py -m pip install pymodbus
import pymodbus.client as ModbusClient
from pymodbus import (
    ExceptionResponse,
    Framer,
    ModbusException,
    pymodbus_apply_logging_config
)
from pymodbus.payload import BinaryPayloadDecoder, BinaryPayloadBuilder
from pymodbus.constants import Endian

DBG_LEVEL = "INFO"
#DBG_LEVEL = "DEBUG"

#COM port
COM_PORT    = "COM16"
DEV_SLAVE   = 1

#Device set values
#SET_R0      = 45
#SET_R1      = 102
SET_R0      = 88
SET_R1      = 99
SET_TS      = 40.00
#SET_ROT     = 0
SET_ROT     = 7

#Modbus map
REG_V1_VER  = 0x200
REG_V1_T    = 0x302
COIL_W      = 0x400
REG_RW      = 0x410
REG_RW_0R0  = 0x410
REG_RW_0R1  = 0x411
REG_RW_ROT  = 0x412
REG_R       = 0x420

#coils array
client = ""
resp = ""
coils_dic = { 0:"start", 1:"heat", 2:"h-boost", 3:"h-still", 4:"motor", 5:"valve", 15:"test"}
coils_status = [0] * (0xF+1)

#2do implement getch() for linux 
def SysKbHit():
    return msvcrt.kbhit()

def SysGetch():
    return msvcrt.getch()

#MAIN
def run_sync_mb_client(com):
    """Run sync client."""
    
    pymodbus_apply_logging_config(DBG_LEVEL)

    global client
    client = ModbusClient.ModbusSerialClient(
    com,
    framer=Framer.RTU,
    baudrate=57600,
    bytesize=8,
    parity="N",
    stopbits=1,
    # timeout=10,
    # retries=3,
    )

    print("Ioffe inst. Graphene eNose V2 client, modbus-" + com + ", z/q-quit, h-help")
    client.connect()
    global resp
    
    try:
        #update coils status
        GetVer()
        GetSetsE()
        GetCoils()
                
        td  = 10
        cyc = 0
        is_run  = True
        is_poll = False
        coil_heat = False
        coil_heat_boost = False

        while(is_run):
            
            if not cyc%td: 
                if is_poll: GetV2Regs()
            cyc+=1

            time.sleep(0.1)
            if SysKbHit(): #windows only
                kb = SysGetch() #print(kb,ikb)
                ikb = int.from_bytes(kb) - 48 #acii to int
                if ikb in coils_dic:
                    SetCoil(ikb)
                if ikb==102-48: SetCoil(0xF)
                    
                match kb:
                    case b'+': td = (td-5) if (td>10) else 10
                    case b'-': td += 5
                    case b'p': is_poll = not is_poll
                    case b'w': SetR0()
                    case b't': SetTs()
                    case b'r': SetRot()
                    case b's': GetSets()
                    case b'c': GetCoils()
                    case b'a': GetCoilsAsReg()
                    case b'v': GetV1Regs()

                    case b'z': is_run = False
                    case b'q': is_run = False

                    case b'h':
                        print("Help: z,q-exit,\
                              \nget: p-toggle poll regs (+/- speed), c,a-coils, s-sets, v-V1 regs,\
                              \nset: 0...9?-coils, w-Ro, t-Ts, r-Rot")
                        for c in coils_dic:
                            print ("%d=%s, " % (c, coils_dic[c]), end=" ")
                        print ("")
                
    except ModbusException as exc:
        print(f"rcv modbus exception({exc})")
        client.close()
        return
    if resp.isError():
        print(f"rcv modbus error({resp})")
        client.close()
        return
    if isinstance(resp, ExceptionResponse):
        print(f"rcv modbus lib exc ({resp})")
        client.close()

    #print("rtu discon")
    client.close()

#Get V1 set temp
def GetVer():
    resp = client.read_holding_registers(REG_V1_VER, 4, DEV_SLAVE)
    if resp.isError(): return
    
    decoder = \
        BinaryPayloadDecoder.fromRegisters(resp.registers, 
            byteorder=Endian.BIG,
            wordorder=Endian.LITTLE)
    
    print("Ver" , decoder.decode_string(8) , end=" " )
    

#Get V1 set temp
def GetSetsE():
    resp = client.read_holding_registers(REG_V1_T, 6, DEV_SLAVE)
    if resp.isError(): return
    print("Ts:%0.2f " % (toSwapFloat(resp.registers[0:2])), end=" " )
    print("V1Ro:%0.2f " % (toSwapFloat(resp.registers[4:6])), end=" " )
    
    resp = client.read_holding_registers(REG_RW, 3, DEV_SLAVE)
    print("V2Ro:%.1f/%.1f  Rot:%d " % (resp.registers[0]/10,resp.registers[1]/10,resp.registers[2]), end=" " )

def GetSets():
    GetSetsE()
    print("")


#Get W coils
def GetCoils():
    print("CoilsON:", end=" ")
    
    global resp
    resp = client.read_holding_registers(COIL_W, 16, DEV_SLAVE)
    #print("<-" , resp.registers )
    
    global coils_dic 
    global coils_status

    for c in coils_dic:
        if (resp.registers[c]): 
            print (coils_dic[c], end=" ")
            coils_status[c] = True
        else:    
            coils_status[c] = False

    print("")

#read coils via 03 cmd
def GetCoilsAsReg():
    global resp
    resp = client.read_holding_registers(COIL_W, 6, DEV_SLAVE)
    if resp.isError(): return
    print("CoilAsReg " , resp.registers )

#Set an writable coil
def SetCoil(c):

    coils_status[c] = not coils_status[c]

    print("Set coil " + coils_dic[c] + "=" + ("on" if coils_status[c] else "off") )
    global resp

    client.write_coil(COIL_W+c, (int)(coils_status[c]==True), DEV_SLAVE)

#Pre set R0 values
def SetR0():
    print("Set V2R0", end="")
    client.write_register(REG_RW_0R0, SET_R0 ,DEV_SLAVE)
    time.sleep(0.5)
    client.write_register(REG_RW_0R1, SET_R1 ,DEV_SLAVE)
    time.sleep(0.5)
    resp = client.read_holding_registers(REG_RW, 2, DEV_SLAVE)
    print("=%.1f/%.1f" % (resp.registers[0]/10, resp.registers[1]/10) )

#Pre set temp
def SetTs():
    #10h multi wr
    print("Set Ts: ", end="")

    builder = BinaryPayloadBuilder(byteorder=Endian.BIG, wordorder=Endian.LITTLE)
    builder.add_32bit_float(SET_TS)
    payload = builder.build()
    client.write_registers(REG_V1_T, payload, DEV_SLAVE, count=2, skip_encode=True)
    
    resp = client.read_holding_registers(REG_V1_T, 2, DEV_SLAVE)
    print("%0.2f" % (toSwapFloat(resp.registers[0:2])))
    #print("<-" , resp.registers )

#Pre set Rotation counter
def SetRot():
    print("Set Rot", end="")
    client.write_register(REG_RW_ROT, SET_ROT ,DEV_SLAVE)
    resp = client.read_holding_registers(REG_RW, 3, DEV_SLAVE)
    print("=%d" % (resp.registers[2]))

#V2 regs got
def GetV2Regs():
    #print("R registers")
    global resp
    resp = client.read_holding_registers(REG_R, 22, DEV_SLAVE)
    if resp.isError(): return
    #print("<~" , resp.registers )
    
    print("Regs: \tHeartB:%d \tRt:%.1f/%.1f \tTr:%.1f/%.1f \tTb: %.1f" % \
            (resp.registers[0], resp.registers[1]/10, resp.registers[2]/10,\
             resp.registers[3]/10, resp.registers[4]/10, resp.registers[5]/10) )

    print("Sens:\t", end='')
    for i in range(6,16,2):
        print("%d:" % (i/2-2), end='')
        if (resp.registers[i]<0xfff0): 
            print("%.1f" % (resp.registers[i]/10), end='')
            if (resp.registers[i+1]!=0):
                print("/%.1f" % (resp.registers[i+1]/10), end='')
            else:print("",end='\t')
            print("",end='\t')
        else:
            if (resp.registers[i]==0xffff): print("empty\t", end='\t')
            if (resp.registers[i]==0xfffE): print("rot\t", end='\t')
    print("")
    
    print("Sen2:", end='')
    if (resp.registers[16] != 0xFFFF): print("\t1:%.2f/%.2f/%.2f" % (resp.registers[16]/100, resp.registers[17]/100, 500+resp.registers[18]/100), end='')
    else: print("\t1:empty", end='')
    if (resp.registers[19] != 0xFFFF): print("\t\t2:%.2f/%.2f/%.2f" % (resp.registers[19]/100, resp.registers[20]/100, 500+resp.registers[21]/100), end='')
    else: print("\t\t2:empty", end='')
    print("")

#V1 regs got
#! seems modbus lib is unstable while get long response
def GetV1Regs():
    #print("V1 regs")
    resp = client.read_holding_registers(0x001, 8, DEV_SLAVE)
    #print("001: " , resp.registers )
    print("V1 MeasN.Ch.dT:%d.%d.%d Tr:%.2f/%.2f Rt:%.2f/%.2f Vref:%.3f" %\
           (resp.registers[0], resp.registers[7], resp.registers[1],\
            resp.registers[2]/100, resp.registers[3]/100,\
            resp.registers[4]/100, resp.registers[5]/100, resp.registers[6]/1000) )
    resp = client.read_holding_registers(0x300, 2, DEV_SLAVE)
    #print("300: " , resp.registers )
    print( "V1 Start=%d, Heat=%d" % (resp.registers[0],resp.registers[1]) )

    resp = client.read_holding_registers(REG_V1_T, 16, DEV_SLAVE)
    if resp.isError(): return
    #print("302:" , resp.registers )
    print("V1 Ts:%0.2f" % (toSwapFloat(resp.registers[0:2])), end=" " )
    print("Rs:%0.2f" % (toSwapFloat(resp.registers[2:4])), end=" " )
    print("Ro:%0.2f" % (toSwapFloat(resp.registers[4:6])), end=" " )
    print("A:%.2g"   % (toSwapFloat(resp.registers[6:8])), end=" " )
    print("B:%.2g"   % (toSwapFloat(resp.registers[8:10])), end=" " )
    print("Kp:%0.2f" % (toSwapFloat(resp.registers[10:12])), end=" " )
    print("Ki:%0.2f" % (toSwapFloat(resp.registers[12:14])), end=" " )
    print("M1:%s%s"  % ( hex(resp.registers[14]), hex(resp.registers[15]) ), end="\n" )
    
    #! here error sometimes, seems unstable modbus lib, cause device works ok
    time.sleep(0.2)
    resp = client.read_holding_registers(0x030, 30, DEV_SLAVE)
    if resp.isError(): 
        print("0x030(R) resp error")
        return
    #print("030:" , resp.registers )
    
    for i in range(0,len(resp.registers),2):
        print("R%d=%.3g " % (1+i/2, toSwapFloat(resp.registers[i:i+2])), end="")
    print("| ", end = "")

    time.sleep(0.2)
    resp = client.read_holding_registers(0x056, 30, DEV_SLAVE)
    if resp.isError(): 
        print("0x056(R) resp error")
        return
    #print("056:" , resp.registers )
    
    for i in range(0,len(resp.registers),2):
        print("R%d=%.3g " % (20+i/2, toSwapFloat(resp.registers[i:i+2])), end="")
    print("")

#RW registers 06 and 10, test only
def TestRegRW():
    global resp
    
    print("RW registers")
    print("-> 06-410h 0R1 0R2: 1234 5678") # 04D2 162E
    
    #06h wr single
    client.write_register(REG_RW_0R0, 1234 ,DEV_SLAVE)
    client.write_register(REG_RW_0R1, 5678 ,DEV_SLAVE)
    resp = client.read_holding_registers(REG_RW, 16, DEV_SLAVE)
    print("<-" , resp.registers )

    #10h multi wr
    client.write_registers(REG_RW, [3456,7890] ,DEV_SLAVE)
    resp = client.read_holding_registers(REG_RW, 16, DEV_SLAVE)
    print("<-" , resp.registers )

def toInt(regs):
    decoder = \
        BinaryPayloadDecoder.fromRegisters(regs, 
            byteorder=Endian.BIG, 
            wordorder=Endian.LITTLE)
    return decoder.decode_16bit_int()


def toSwapFloat(regs):
    #4.2=0x40866666, swapped 86 40 
    swp = [0] * 2
    swp[0] = struct.unpack("<H", struct.pack(">H", regs[0]))[0]
    swp[1] = struct.unpack("<H", struct.pack(">H", regs[1]))[0]
    decoder = \
        BinaryPayloadDecoder.fromRegisters(swp, 
            byteorder=Endian.BIG, 
            wordorder=Endian.LITTLE)
    return decoder.decode_32bit_float()


if __name__ == "__main__":
    run_sync_mb_client(COM_PORT)
