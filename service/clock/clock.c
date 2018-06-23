/*******************************************************************************
 *  Copyright(C)2018 by Dreistein<mcu_shilei@hotmail.com>                     *
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

//! \note do not move this pre-processor statement to other places
#define __SERVICE_CLOCK_C__

//! \brief normal precision ac that count in millisecond. abstract: a clock 
//!        with only one hand which has a one-day cycle.

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\clock_public.h"

/*============================ MACROS ========================================*/
#define CLOCK_TICKS_A_DAY   (24u * 60u * 60u)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct {
    uint32_t        Hand;
    list_node_t     AlarmListToday;
    list_node_t     AlarmListNextDay;
} real_clock_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static real_clock_t realClock;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
static void __clock_list_insert(list_node_t *pList, alarmclock_t *ac)
{
    alarmclock_t *pAC;
    list_node_t  *pNode;

    for (pNode = pList->Next; pNode != pList; pNode = pNode->Next) {
        pAC = CONTAINER_OF(pNode, alarmclock_t, ListNode);
        if (ac->Count < pAC->Count) {
            break;
        }
    }
    list_add(&ac->ListNode, pNode->Prev);
}

static void clock_list_insert(alarmclock_t *ac)
{
    list_node_t *pList;

    //! ac->Count forward realClock.Hand?
    if (ac->Count > realClock.Hand) {   //! yes.
        pList = &realClock.AlarmListToday;
    } else {                            //! no.
        pList = &realClock.AlarmListNextDay;
    }
    __clock_list_insert(pList, ac);
}

static void clock_list_remove(alarmclock_t *ac)
{
    list_del(&ac->ListNode);
}

static void clock_alarm_process(alarmclock_t *ac)
{
    clock_list_remove(ac);
    clock_list_insert(ac);
    ac->Flag |= BIT(0);
    if (ac->pRoutine != NULL) {
        ac->pRoutine();
    }
}

//! This function should be called periodly.
void clock_tick_tock(void)
{
    __CLOCK_SAFE_ATOM_CODE(
        //! increase realClock.Hand
        ++realClock.Hand;
        if (realClock.Hand >= CLOCK_TICKS_A_DAY) {
            realClock.Hand = 0u;
        }

        //! to see if it has run over.
        if (0u == realClock.Hand) { //! yes.            
            //! move realClock.AlarmListNextDay to realClock.AlarmListToday.
            list_node_t *pHead = realClock.AlarmListNextDay.Next;
            list_node_t *pTail = realClock.AlarmListNextDay.Prev;
            realClock.AlarmListNextDay.Next = &realClock.AlarmListNextDay;
            realClock.AlarmListNextDay.Prev = &realClock.AlarmListNextDay;
            realClock.AlarmListToday.Next = pHead;
            realClock.AlarmListToday.Prev = pTail;
            pHead->Prev = &realClock.AlarmListToday;
            pTail->Next = &realClock.AlarmListToday;
        }

        //! to see if there is any ac overflow in realClock.AlarmListToday.
        for (list_node_t *pNode = realClock.AlarmListToday.Next; pNode != &realClock.AlarmListToday; ) {
            alarmclock_t *pAC = CONTAINER_OF(pNode, alarmclock_t, ListNode);
            //! to see if it has overflow.
            if (pAC->Count > realClock.Hand) { //!< no.
                break;            //!< The list has been sorted, so we just break.
            } else {                        //!< yes
                pNode = pNode->Next;
                clock_alarm_process(pAC);
            }
        }
    )
}

bool clock_init(void)
{
    realClock.Hand = 0u;
    list_init_head(&realClock.AlarmListToday);
    list_init_head(&realClock.AlarmListNextDay);
    return true;
}

bool clock_set_time(uint32_t time)
{
    realClock.Hand = time;
    //! considering we may turn hand forward, so:
    //! remove all thoese alarms which before current Hand from AlarmListToday to AlarmListNextDay.
    //! considering we may turn hand backward, so:
    //! remove all thoese alarms which after current Hand from AlarmListNextDay to AlarmListToday.
    return true;
}

bool alarmclock_config(
    alarmclock_t           *ac,
    uint32_t                value,
    alarmclock_routine_t   *pRoutine)
{
    value %= CLOCK_TICKS_A_DAY;
    ac->Flag     = 0;
    ac->pRoutine = pRoutine;
    list_init_head(&ac->ListNode);
    //! start it.
    __CLOCK_SAFE_ATOM_CODE(
        ac->Count = value;
        clock_list_insert(ac);
    )

    return true;
}

void alarmclock_start(alarmclock_t *ac, uint32_t value)
{
    value %= CLOCK_TICKS_A_DAY;
    __CLOCK_SAFE_ATOM_CODE(
        //! remove it from running list.
        clock_list_remove(ac);
        ac->Count = value;
        //! start it again.
        clock_list_insert(ac);
    )
}

void alarmclock_stop(alarmclock_t *ac)
{
    __CLOCK_SAFE_ATOM_CODE(
        //! remove it from running list.
        clock_list_remove(ac);
    )
}

bool alarmclock_is_timeout(alarmclock_t *ac)
{
    if (ac->Flag) {
        ac->Flag &= ~BIT(0);
        return true;
    }

    return false;
}



/* EOF */
