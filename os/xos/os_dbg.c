
#include ".\source\os.h"

/*!
 *! DEBUG DATA
 */
#if OS_DEBUG_EN > 0u

const UINT16  OSDebugEn           = OS_DEBUG_EN;               //!< Debug constants are defined below  

const UINT32  OSEndiannessTest    = 0x00000001u;               //!< Variable to test CPU endianness    

const UINT16  OSSempEn           = OS_SEMP_EN;
const UINT16  OSSempMax          = OS_MAX_SEMAPHORES;             //!< Number of event control blocks     
#if (OS_SEMP_EN) && (OS_MAX_SEMAPHORES > 0u)
const UINT16  OSSempSize         = sizeof(OS_SEMP);          //!< Size in Bytes of OS_EVENT          
#else
const UINT16  OSSempSize         = 0u;
#endif

const UINT16  OSMutexEn           = OS_MUTEX_EN;
const UINT16  OSMutexMax          = OS_MAX_MUTEXES;             //!< Number of event control blocks     
#if (OS_MUTEX_EN) && (OS_MAX_MUTEXES > 0u)
const UINT16  OSMutexSize         = sizeof(OS_MUTEX);          //!< Size in Bytes of OS_EVENT          
#else
const UINT16  OSMutexSize         = 0u;
#endif

const UINT16  OSFlagEn            = OS_FLAG_EN;
const UINT16  OSFlagMax           = OS_MAX_FLAGS;
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
const UINT16  OSFlagSize          = sizeof(OS_FLAG);           //!< Size in Bytes of OS_FLAG       
#else
const UINT16  OSFlagSize          = 0u;
#endif

const UINT16  OSWaitNodeSize      = sizeof(OS_WAIT_NODE);      //!< Size in Bytes of OS_WAIT_NODE      

const UINT16  OSLowestPrio        = OS_MAX_PRIO_LEVELS;

const UINT16  OSPtrSize           = sizeof(void *);            //!< Size in Bytes of a pointer         

const UINT16  OSRdyTblSize        = OS_BITMAP_TBL_SIZE;        //!< Number of bytes in the ready table 

const UINT16  OSStkWidth          = sizeof(OS_STK);            //!< Size in Bytes of a stack entry     

const UINT16  OSTaskIdleStkSize   = OS_TASK_IDLE_STK_SIZE;
const UINT16  OSTaskProfileEn     = OS_TASK_PROFILE_EN;
const UINT16  OSTaskMax           = OS_MAX_TASKS + OS_N_SYS_TASKS; //!< Total max. number of tasks     
const UINT16  OSTaskStatEn        = OS_STAT_EN;
const UINT16  OSTaskStatStkSize   = OS_TASK_STAT_STK_SIZE;
const UINT16  OSTaskStatStkChkEn  = OS_STAT_TASK_STK_CHK_EN;
const UINT16  OSTaskSwHookEn      = OS_TASK_SW_HOOK_EN;

const UINT16  OSTCBPrioTblMax     = OS_MAX_PRIO_LEVELS;       //!< Number of entries in osPrioTCBTbl[]
const UINT16  OSTCBSize           = sizeof(OS_TCB);            //!< Size in Bytes of OS_TCB            

const UINT16  OSTicksPerSec       = OS_TICKS_PER_SEC;
const UINT16  OSTimeTickHookEn    = OS_TIME_TICK_HOOK_EN;

const UINT16  OSVersionNbr        = OS_VERSION;


/*!
 *! TOTAL DATA SPACE (in RAM) USED BY OS
 */
const UINT16  OSDataSize =
#if (OS_SEMP_EN) && (OS_MAX_SEMAPHORES > 0u)
                          + sizeof(osSempFreeTbl)
                          + sizeof(osSempFreeList)
#endif

#if (OS_MUTEX_EN) && (OS_MAX_MUTEXES > 0u)
                          + sizeof(osMutexFreeTbl)
                          + sizeof(osMutexFreeList)
#endif
                              
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
                          + sizeof(osFlagFreeTbl)
                          + sizeof(osFlagFreeList)
#endif
#if OS_STAT_EN > 0u
                          + sizeof(osTaskCtr)
                          + sizeof(osCtxSwCtr)
                          + sizeof(osCPUUsage)
                          + sizeof(osIdleCtrMax)
                          + sizeof(osStatRunning)
                          + sizeof(osTaskStatStk)
#endif
                          + sizeof(osIntNesting)
                          + sizeof(osLockNesting)
                          + sizeof(osRunning)
                          + sizeof(osIdleCtr)
                          + sizeof(osTaskIdleStk)
                          + sizeof(osTCBCur)
                          + sizeof(osTCBHighRdy)
                          + sizeof(osTCBFreeTbl)
                          + sizeof(osTCBFreeList)
                          + sizeof(osRdyGrp)
                          + sizeof(osRdyTbl)
                          + sizeof(osRdyList)
                          + sizeof(osPndList)
                          ;
#endif

/*!
 *!                                        OS DEBUG INITIALIZATION
 *!
 *! \Description This function is used to make sure that debug variables that are unused in the application
 *!              are not optimized away.  This function might not be necessary for all compilers.  In this
 *!              case, you should simply DELETE the code in this function while still leaving the declaration
 *!              of the function itself.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Notes       (1) This code doesn't do anything, it simply prevents the compiler from optimizing out
 *!                  the 'const' variables which are declared in this file.
 *!              (2) You may decide to 'compile out' the code (by using #if 0/#endif) INSIDE the function 
 *!                  if your compiler DOES NOT optimize out the 'const' variables above.
 */

#if OS_DEBUG_EN > 0u
void OSDebugInit(void)
{
    void  const  *ptemp;


    ptemp = (void const *)&OSDebugEn;

    ptemp = (void const *)&OSEndiannessTest;

    ptemp = (void const *)&OSMutexMax;
    ptemp = (void const *)&OSMutexEn;
    ptemp = (void const *)&OSMutexSize;

    ptemp = (void const *)&OSFlagEn;
    ptemp = (void const *)&OSFlagSize;
    ptemp = (void const *)&OSWaitNodeSize;
    ptemp = (void const *)&OSFlagMax;

    ptemp = (void const *)&OSLowestPrio;

    ptemp = (void const *)&OSMutexEn;

    ptemp = (void const *)&OSPtrSize;

    ptemp = (void const *)&OSRdyTblSize;

    ptemp = (void const *)&OSSempEn;

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
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
