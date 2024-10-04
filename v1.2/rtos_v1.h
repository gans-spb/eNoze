//
 
#define MAXnTASKS       30   // 
 

typedef struct task
{
   void (*pFunc) (void);                    // 
   volatile unsigned int delay;             // 
   volatile unsigned int period;            // 
   volatile unsigned char run;              // 
}task;


extern void RTOS_Init (void);
extern void RTOS_SetTask (void (*taskfunc)(void), unsigned int taskperiod, unsigned int taskdelay);
extern void RTOS_Dispatch(void);
extern void RTOS_DelTask (void (*taskFunc)(void));
extern void RTOS_timer(void);
