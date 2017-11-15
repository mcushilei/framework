/*******************************************************************************
 *  Copyright(C)2015-2017 by Dreistein<mcu_shilei@hotmail.com>                *
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
#define __LIST_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\list_public.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void list_init_head(list_node_t *head)
{
    head->Next = head;
    head->Prev = head;
}

static void __list_add(list_node_t *node, list_node_t *prev, list_node_t *next)
{
    next->Prev = node;
    node->Next = next;
    node->Prev = prev;
    prev->Next = node;
}

void list_add(list_node_t *node, list_node_t *head)
{
    __list_add(node, head, head->Next);
}
 
 
static void __list_del(list_node_t *prev, list_node_t *next)
{
    next->Prev = prev;
    prev->Next = next;
}
 
void list_del(list_node_t *entry)
{
    __list_del(entry->Prev, entry->Next);
    entry->Next = entry;
    entry->Prev = entry;
}


/* EOF */
