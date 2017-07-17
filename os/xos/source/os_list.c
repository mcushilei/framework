
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
void os_list_init_head(OS_LIST_NODE *Head)
{
    Head->Next = Head;
    Head->Prev = Head;
}

static void __os_list_add(OS_LIST_NODE *Node, OS_LIST_NODE *Prev, OS_LIST_NODE *Next)
{
    Next->Prev = Node;
    Node->Next = Next;
    Node->Prev = Prev;
    Prev->Next = Node;
}

void os_list_add(OS_LIST_NODE *Node, OS_LIST_NODE *Head)
{
    __os_list_add(Node, Head, Head->Next);
}
 
 
void __os_list_del(OS_LIST_NODE* Prev, OS_LIST_NODE* Next)
{
    Next->Prev = Prev;
    Prev->Next = Next;
}
 
void os_list_del(OS_LIST_NODE *entry)
{
    __os_list_del(entry->Prev, entry->Next);
    entry->Next = NULL;
    entry->Prev = NULL;
}


/* EOF */
