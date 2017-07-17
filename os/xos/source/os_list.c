
//! \note do not move this pre-processor statement to other places
#define __OS_LIST_C__

/*============================ INCLUDES ======================================*/
#include ".\os.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void os_list_init_head(OS_LIST_NODE *head)
{
    head->Next = head;
    head->Prev = head;
}

static void __os_list_add(OS_LIST_NODE *node, OS_LIST_NODE *prev, OS_LIST_NODE *next)
{
    next->Prev = node;
    node->Next = next;
    node->Prev = prev;
    prev->Next = node;
}

void os_list_add(OS_LIST_NODE *node, OS_LIST_NODE *head)
{
    __os_list_add(node, head, head->Next);
}
 
 
void __os_list_del(OS_LIST_NODE *prev, OS_LIST_NODE *next)
{
    next->Prev = prev;
    prev->Next = next;
}
 
void os_list_del(OS_LIST_NODE *entry)
{
    __os_list_del(entry->Prev, entry->Next);
    entry->Next = NULL;
    entry->Prev = NULL;
}


/* EOF */
