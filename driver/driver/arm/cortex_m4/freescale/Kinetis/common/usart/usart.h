#ifndef __USART1_H__
#define __USART1_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\i_io_usart.h"
#include "..\device.h"

/*============================ MACROS ========================================*/                              

#define USART_CFG(__USART, ...)                                             \
        do {                                                                \
            usart_cfg_t tCFG = {__VA_ARGS__};                               \
            __USART.Init(&tCFG);                                            \
        } while(0)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name usart working mode 
//! @{
typedef enum {
    USART_NO_PARITY         = 0x00,
    USART_EVEN_PARITY       = 0x02,
    USART_ODD_PARITY        = 0x03,

    USART_1_STOPBIT         = 0x00,

    USART_8_BIT_LENGTH      = 0x00,
}em_usart_mode_t;
//! @}

//! \name usart configuration
//! @{
typedef struct {
    uint16_t            hwMode;
    uint32_t            wBaudrate;
} usart_cfg_t;
//! @}


//! \name class: usart_t
//! @{
DEF_INTERFACE(usart_t)
    //usart_reg_t *const RegPage;
    bool        (*Init)(usart_cfg_t *ptCFG);
    bool        (*Idle)(void);
    bool        (*Enable)(void);
    bool        (*Disable)(void);
    bool        (*WriteByte)(uint8_t chByte);
    bool        (*ReadByte)(uint8_t *pchByte);
    
    u32_property_t Baudrate; 

END_DEF_INTERFACE(usart_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const usart_t USART[USART_COUNT];

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
