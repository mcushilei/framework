/***************************************************************************
 *   Copyright(C)2009-2012 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
#define LOCKED          true            //!< locked
#define UNLOCKED        false           //!< unlocked

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef volatile bool locker_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief try to enter a section
 *! \param ptLock locker object
 *! \retval lock section is entered
 *! \retval The section is locked
 */
bool enter_lock(locker_t *ptLock)
{
    bool bResult = false;

    if (NULL == ptLock) {
        return true;
    }

    SAFE_ATOM_CODE(
        if (!(*ptLock)) {
            (*ptLock) = LOCKED;
            bResult = true;
        }
    )
        
    return bResult;
}


/*! \brief leave a section
 *! \param ptLock locker object
 *! \return none
 */
void leave_lock(locker_t *ptLock)
{
    if (NULL == ptLock) {
        return;
    }

    SAFE_ATOM_CODE(
        (*ptLock) = UNLOCKED;
    )
}

/* EOF */
