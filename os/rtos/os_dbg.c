
#include ".\source\os.h"

/*********************************************************************************************************
*                                             DEBUG DATA
*********************************************************************************************************/

INT16U  const  OSDebugEn           = OS_DEBUG_EN;               //!< Debug constants are defined below  

#if OS_DEBUG_EN > 0u

INT32U  const  OSEndiannessTest    = 0x00000001u;               //!< Variable to test CPU endianness    

INT16U  const  OSEventEn           = OS_EVENT_EN;
INT16U  const  OSEventMax          = OS_MAX_EVENTS;             //!< Number of event control blocks     
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
INT16U  const  OSEventSize         = sizeof(OS_EVENT);          //!< Size in Bytes of OS_EVENT          
#else
INT16U  const  OSEventSize         = 0u;
#endif

INT16U  const  OSFlagEn            = OS_FLAG_EN;
INT16U  const  OSFlagMax           = OS_MAX_FLAGS;
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
INT16U  const  OSFlagSize          = sizeof(OS_FLAG);           //!< Size in Bytes of OS_FLAG       
INT16U  const  OSFlagNodeSize      = sizeof(OS_FLAG_NODE);      //!< Size in Bytes of OS_FLAG_NODE      
#else
INT16U  const  OSFlagSize          = 0u;
INT16U  const  OSFlagNodeSize      = 0u;
#endif

INT16U  const  OSLowestPrio        = OS_MAX_PRIO_LEVELS;

INT16U  const  OSMutexEn           = OS_MUTEX_EN;

INT16U  const  OSPtrSize           = sizeof(void *);            //!< Size in Bytes of a pointer         

INT16U  const  OSRdyTblSize        = OS_RDY_TBL_SIZE;           //!< Number of bytes in the ready table 

INT16U  const  OSSemEn             = OS_SEM_EN;

INT16U  const  OSStkWidth          = sizeof(OS_STK);            //!< Size in Bytes of a stack entry     

INT16U  const  OSTaskIdleStkSize   = OS_TASK_IDLE_STK_SIZE;
INT16U  const  OSTaskProfileEn     = OS_TASK_PROFILE_EN;
INT16U  const  OSTaskMax           = OS_MAX_TASKS + OS_N_SYS_TASKS; //!< Total max. number of tasks     
INT16U  const  OSTaskStatEn        = OS_STAT_EN;
INT16U  const  OSTaskStatStkSize   = OS_TASK_STAT_STK_SIZE;
INT16U  const  OSTaskStatStkChkEn  = OS_STAT_TASK_STK_CHK_EN;
INT16U  const  OSTaskSwHookEn      = OS_TASK_SW_HOOK_EN;

INT16U  const  OSTCBPrioTblMax     = OS_MAX_PRIO_LEVELS;       //!< Number of entries in OSTaskPrioTCBTbl[]
INT16U  const  OSTCBSize           = sizeof(OS_TCB);            //!< Size in Bytes of OS_TCB            

INT16U  const  OSTicksPerSec       = OS_TICKS_PER_SEC;
INT16U  const  OSTimeTickHookEn    = OS_TIME_TICK_HOOK_EN;

INT16U  const  OSVersionNbr        = OS_VERSION;

#endif

/*********************************************************************************************************
*                                             DEBUG DATA
*                            TOTAL DATA SPACE (in RAM) USED BY OS
*********************************************************************************************************/
#if OS_DEBUG_EN > 0u

INT16U  const  OSDataSize =
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
                          + sizeof(OSEventFreeList)
                          + sizeof(OSEventFreeTbl)
#endif
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
                          + sizeof(OSFlagFreeTbl)
                          + sizeof(OSFlagFreeList)
#endif
#if OS_STAT_EN > 0u
                          + sizeof(OSCtxSwCtr)
                          + sizeof(OSCPUUsage)
                          + sizeof(OSIdleCtrMax)
                          + sizeof(OSStatRunning)
                          + sizeof(OSTaskStatStk)
#endif
                          + sizeof(OSIntNesting)
                          + sizeof(OSLockNesting)
                          + sizeof(OSRdyGrp)
                          + sizeof(OSRdyTbl)
                          + sizeof(OSRunning)
                          + sizeof(OSTaskCtr)
                          + sizeof(OSIdleCtr)
                          + sizeof(OSTaskIdleStk)
                          + sizeof(OSTCBCur)
                          + sizeof(OSTCBFreeList)
                          + sizeof(OSTCBHighRdy)
                          + sizeof(OSTaskList)
                          + sizeof(OSTaskPrioTCBTbl)
                          + sizeof(OSTCBFreeTbl);
#endif

/*********************************************************************************************************
*                                        OS DEBUG INITIALIZATION
*
* Description: This function is used to make sure that debug variables that are unused in the application
*              are not optimized away.  This function might not be necessary for all compilers.  In this
*              case, you should simply DELETE the code in this function while still leaving the declaration
*              of the function itself.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : (1) This code doesn't do anything, it simply prevents the compiler from optimizing out
*                  the 'const' variables which are declared in this file.
*              (2) You may decide to 'compile out' the code (by using #if 0/#endif) INSIDE the function 
*                  if your compiler DOES NOT optimize out the 'const' variables above.
*********************************************************************************************************/

#if OS_DEBUG_EN > 0u
void  OSDebugInit (void)
{
    void  const  *ptemp;


    ptemp = (void const *)&OSDebugEn;

    ptemp = (void const *)&OSEndiannessTest;

    ptemp = (void const *)&OSEventMax;
    ptemp = (void const *)&OSEventEn;
    ptemp = (void const *)&OSEventSize;

    ptemp = (void const *)&OSFlagEn;
    ptemp = (void const *)&OSFlagSize;
    ptemp = (void const *)&OSFlagNodeSize;
    ptemp = (void const *)&OSFlagMax;

    ptemp = (void const *)&OSLowestPrio;

    ptemp = (void const *)&OSMutexEn;

    ptemp = (void const *)&OSPtrSize;

    ptemp = (void const *)&OSRdyTblSize;

    ptemp = (void const *)&OSSemEn;

    ptemp = (void const *)&OSStkWidth;

    ptemp = (void const *)&OSTaskIdleStkSize;
    ptemp = (void const *)&OSTaskProfileEn;
    ptemp = (void const *)&OSTaskMax;
    ptemp = (void const *)&OSTaskStatEn;
    ptemp = (void const *)&OSTaskStatStkSize;
    ptemp = (void const *)&OSTaskStatStkChkEn;
    ptemp = (void const *)&OSTaskSwHookEn;

    ptemp = (void const *)&OSTCBPrioTblMax;
    ptemp = (void const *)&OSTCBSize;

    ptemp = (void const *)&OSTicksPerSec;
    ptemp = (void const *)&OSTimeTickHookEn;

    ptemp = (void const *)&OSVersionNbr;

    ptemp = (void const *)&OSDataSize;

    ptemp = ptemp;                             //!< Prevent compiler warning for 'ptemp' not being used!
}
#endif
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
