/*******************************************************************************
 *  Copyright(C)2015 by Dreistein<mcu_shilei@hotmail.com>                     *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify it   *
 *  under the terms of the GNU Lesser General Public License as published     *
 *  by the Free Software Foundation; either version 3 of the License, or      *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful, but       *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          *
 *  General Public License for more details.                                  *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with this program; if not, see http://www.gnu.org/licenses/.        *
*******************************************************************************/


/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\common\device.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
AT_ADDR(0x10003F10) NO_INIT ROOT struct {
    unsigned int R0;
    unsigned int R1;
    unsigned int R2;
    unsigned int R3;
    unsigned int R12;
    unsigned int LR;
    unsigned int PC;
    unsigned int xPSR;

    unsigned char MFSR;
    unsigned char BFSR;
    unsigned short UFSR;
    unsigned int HFSR;
    unsigned int DFSR;
    unsigned int AFSR;
    unsigned int MMAR;
    unsigned int BFAR;
} err_log;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**************************************************************
 **
 ** How to implement a low-level initialization function in C
 ** =========================================================
 **
 ** 1) Only use local auto variables.
 ** 2) Don't use global or static variables.
 ** 3) Don't use global or static objects (EC++ only).
 ** 4) Don't use agregate initializers, e.g. struct a b = {1};
 ** 5) Don't call any library functions (function calls that
 **    the compiler generates, e.g. to do integer math, are OK).
 ** 6) Setup the RSTACK as is appropriate!
 **    See code below or use the command line --enable_external_bus
 **
 **************************************************************/
#if SYSTEM_LOW_LEVEL_INIT == ENABLED
WEAK uint_fast8_t BEFORE_SYSTEM_INIT(void)
{
    /* Return 1 to indicate that normal segment */
    /* initialization should be performed. If   */
    /* normal segment initialization should not */
    /* be performed, return 0.                  */
    return 1;
}

ROOT uint_fast8_t __low_level_init(void)
{
    /* Uncomment the statement below if the RSTACK */
    /* segment has been placed in external SRAM!   */

    /* __require(__RSTACK_in_external_ram); */
    /* Or */
    /* __require(__RSTACK_in_external_ram_new_way); */
    /* Plus, if you uncomment this you will have to */
    /* define the sfr __?XMCRA in assembler         */

    /* If the low-level initialization routine is  */
    /* written in assembler, the line above should */
    /* be written as:                              */
    /*     EXTERN  __RSTACK_in_external_ram        */
    /*     REQUIRE __RSTACK_in_external_ram        */

    /* Add your custom setup here. */

    return BEFORE_SYSTEM_INIT();;
}
#endif

//! \brief default hardware init routine
WEAK bool ON_HW_INIT(void)
{
    return true;
}

/*! \note initialize driver
 *  \param none
 *  \retval true hal initialization succeeded.
 *  \retval false hal initialization failed
 */  
bool driver_init( void )
{
    if (!ON_HW_INIT()) {
        return false;
    }
    
    return true;
}

/* EOF */
