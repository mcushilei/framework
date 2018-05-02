/*******************************************************************************
*  Copyright(C)2016-2018 by Dreistein<mcu_shilei@hotmail.com>                *
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
#define __SERVICE_TIMER_C__

//! \brief normal precision timer that count in millisecond.

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\timer_public.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static volatile uint32_t    scanHand;
static volatile uint32_t    scanHandOld;
static list_node_t          timerList;            //! those two list should be sorted by increase. 
static list_node_t          timerRunoverList;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

bool timer_init(void)
{
    scanHandOld = scanHand;
    list_init_head(&timerList);
    list_init_head(&timerRunoverList);
    return true;
}

static void timer_list_insert(timer_t *timer)
{
    list_node_t *pList;

    //! to see if we have run over.
    if (scanHandOld > scanHand) { //! yes.
        if (timer->Count > scanHand) {
            pList = &timerRunoverList;
        } else {
            pList = &timerList;
        }
    } else {
        if (timer->Count > scanHand) {
            pList = &timerList;
        } else {
            pList = &timerRunoverList;
        }
    }

    //! is this list empty?
    if (pList->Next == pList) {     //! yes.
        list_add(&timer->ListNode, pList);
    } else {                        //! no.
        timer_t *pTimer;
        list_node_t *pNode;

        for (pNode = pList->Next; pNode != pList; pNode = pNode->Next) {
            pTimer = CONTAINER_OF(pNode, timer_t, ListNode);
            if (timer->Count < pTimer->Count) {
                break;
            }
        }
        list_add(&timer->ListNode, pNode->Prev);
    }
}

static void timer_list_remove(timer_t *timer)
{
    //! remove it.
    list_del(&timer->ListNode);
}

static void timer_timeout_processs(timer_t *timer)
{
    timer->Flag |= BIT(0);
    if (timer->pRoutine != NULL) {
        timer->pRoutine();
    }
    if (timer->Period != 0u) {
        timer->Count = scanHand + timer->Period;
        timer_list_insert(timer);
    }
}

//! This function should be called periodly.
void timer_tick(void)
{
    //! increase scanHand
    ++scanHand;

    __TIMER_SAFE_ATOM_CODE(
        //! to see if we have run over.
        if (scanHandOld > scanHand) { //! yes.
            //! all timer in timerList has timeout.
            if (timerList.Next != &timerList) {    //! see if timerList is empyt.
                for (list_node_t *pNode = timerList.Next; pNode != &timerList; ) {
                    timer_t *pTimer = CONTAINER_OF(pNode, timer_t, ListNode);
                    pNode = pNode->Next;
                    list_del(&pTimer->ListNode);
                    timer_timeout_processs(pTimer);
                }
            }

            //! move timerRunoverList to timerList.
            if (timerRunoverList.Next != &timerRunoverList) {    //! see if timerRunoverList is empty.
                list_node_t *pHead = timerRunoverList.Next;
                list_node_t *pTail = timerRunoverList.Prev;
                timerRunoverList.Next = &timerRunoverList;
                timerRunoverList.Prev = &timerRunoverList;
                timerList.Next = pHead;
                timerList.Prev = pTail;
                pHead->Prev = &timerList;
                pTail->Next = &timerList;
            }
        }

        //! to see if there is any timer overflow in timerList.
        if (timerList.Next != &timerList) {    //! see if timerList is empyt.
            for (list_node_t *pNode = timerList.Next; pNode != &timerList; ) {
                timer_t *pTimer = CONTAINER_OF(pNode, timer_t, ListNode);
                //! to see if it has overflow.
                if (pTimer->Count > scanHand) { //!< no.
                    break;            //!< The list has been sorted, so we just break.
                } else {                        //!< yes
                    pNode = pNode->Next;
                    list_del(&pTimer->ListNode);
                    timer_timeout_processs(pTimer);
                }
            }
        }
    )

    scanHandOld = scanHand;
}

//! count in millisecond.
//! NO reentrible
bool timer_config(
    timer_t        *timer,
    uint32_t        initValue,
    uint32_t        reloadValue,
    timer_routine_t *pRoutine)
{
    timer->Count  = (initValue   + TIMER_TICK_CYCLE - 1u) / TIMER_TICK_CYCLE;
    timer->Period = (reloadValue + TIMER_TICK_CYCLE - 1u) / TIMER_TICK_CYCLE;
    timer->Flag   = 0;
    timer->pRoutine = pRoutine;
    list_init_head(&timer->ListNode);
    if (timer->Count != 0u) {
        timer->Count += scanHand;
        __TIMER_SAFE_ATOM_CODE(
            timer_list_insert(timer);
        )
    }

    return true;
}

//! NO reentrible
void timer_start(timer_t *timer, uint32_t value)
{
    __TIMER_SAFE_ATOM_CODE(
        //! remove it from any list.
        timer_list_remove(timer);
        //! update it and then add it to list again.
        if (value != 0u) {
            timer->Count = (value + TIMER_TICK_CYCLE - 1u) / TIMER_TICK_CYCLE + scanHand;
            timer_list_insert(timer);
        }
    )
}

//! NO reentrible
void timer_stop(timer_t *timer)
{
    __TIMER_SAFE_ATOM_CODE(
        //! remove it from any list.
        timer_list_remove(timer);
    )
}

bool timer_is_timeout(timer_t *timer)
{
    if (timer->Flag) {
        timer->Flag &= ~BIT(0);
        return true;
    }

    return false;
}

bool timer_is_running(timer_t *timer)
{
    if (timer->ListNode.Next != &timer->ListNode) {
        return true;
    }
    return false;
}


/* EOF */
