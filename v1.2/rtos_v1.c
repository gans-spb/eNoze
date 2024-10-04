
#include "rtos_v1.h"
#include "timer.h"

volatile task TaskArray[MAXnTASKS];     /// a task array  


///////////////// reset a function parameters  ///////////////////////
static void DeleteTask (unsigned char num)
{
   if(num<MAXnTASKS)
   {
     TaskArray[num].pFunc = 0x0000;   // a pionter of function
     TaskArray[num].delay = 0;        // a delay before first start
     TaskArray[num].period = 0;       // a period of start
     TaskArray[num].run = 0;          // a flag of run
   }
}
/////////////////////////// system timer ////////////////////////////
extern void RTOS_timer(void)
{
   unsigned char i;
   
   if(get_time_sys(RTOS_TIME))
   {
       for (i=0; i<MAXnTASKS; i++)
       {
          if (TaskArray[i].pFunc)
          {  
             //
             if (TaskArray[i].delay <= 1) 
             {
                //
                TaskArray[i].run = 1;
                TaskArray[i].delay = TaskArray[i].period;
             }
             else TaskArray[i].delay--;
          }
       }
   }
}
////////////////////////////////////////////////////////////////////
extern void RTOS_Init(void)
{
  unsigned char i;
   
  // clear a task array   
  for (i=0; i<MAXnTASKS; i++) DeleteTask(i);
  
}
////////////////////////// set a task to a sheduler /////////////////////////
extern void RTOS_SetTask(void (*taskfunc)(void), unsigned int taskperiod, unsigned int taskdelay)
{
  unsigned char i;  
  unsigned char j=0; 
  unsigned char flag=0; 
   
   for (i=0; i<MAXnTASKS; i++)            // 
   {
       if(TaskArray[i].pFunc == taskfunc) // 
	   { 
     
          TaskArray[i].delay = taskdelay;
          TaskArray[i].period = taskperiod;
          TaskArray[i].run = 0;    
          
          flag=1;
       }
   }
   if(!flag)
   {
       while ((TaskArray[j].pFunc != 0) && (j < MAXnTASKS)) { j++; }
     
       // 
       if (j < MAXnTASKS)
       {
          TaskArray[j].pFunc = taskfunc;
          TaskArray[j].delay = taskdelay;
          TaskArray[j].period = taskperiod;
          TaskArray[j].run = 0; 
       }   
   }
}
////////////////////////////////////////////////////////////////////////
extern void RTOS_DelTask (void (*taskFunc)(void))
{
   unsigned char i;
  
   for (i=0; i<MAXnTASKS; i++)             
   {
      if(TaskArray[i].pFunc == taskFunc)  
      {
        DeleteTask(i);
      }
   } 
}
///////////////////////////////////////////////////////////////////////
extern void RTOS_Dispatch(void)
{
   unsigned char i;
 
   for (i=0; i<MAXnTASKS; i++)
   {
      if (TaskArray[i].run == 1)
      {     
         (*TaskArray[i].pFunc)();                //   
         
         TaskArray[i].run = 0;                   //    
         if(!TaskArray[i].period) DeleteTask(i); // 
      }
   } 
} 
