
/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\i_io_usart.h"
#include "..\device.h"

/*============================ MACROS ========================================*/

//!< \brief <TBD>
#define SAFE_CLK_CODE(...)                                                  \
    {                                                                       \
        uint32_t wMask = SIM_SCGC_UART0_MASK << ptThis->chIndex;            \
        uint32_t wTemp =    SIM_SCGC_UART & wMask;                          \
        SIM_SCGC_UART |= SIM_SCGC_UART0_MASK << (ptThis->chIndex);          \
        __VA_ARGS__;                                                        \
        if (!wTemp) {                                                       \
            SIM_SCGC_UART &= ~wMask;                                        \
        }                                                                   \
    }

//!< \brief <TBD>
//#define __USART_FUNCTION(_N, _A)                                            \
//    {                                                                       \
//        ((usart_reg_t *)(USART##_N##_BASE_ADDRESS)),                        \
//        AHBCLK_USART##_N,                                                   \
//        PCLK_USART##_N,                                                     \
//    },

#define __USART_FUNCTION(_N, _A)                                            \
    {                                                                       \
        ((usart_reg_t *)(USART##_N##_BASE_ADDRESS)),                        \
        _N,                                                                 \
    },

#define USART_FUNCTION(_N, _A)                                              \
    {                                                                       \
        .Init = &usart##_N##_init,                                          \
        .Idle = &usart##_N##_idle,                                          \
        .Enable = &usart##_N##_enable,                                      \
        .Disable = &usart##_N##_disable,                                    \
        .WriteByte = &usart##_N##_write_byte,                               \
        .ReadByte = &usart##_N##_read_byte,                                 \
        .Baudrate = {                                                       \
            .Set = &usart##_N##_set,                                        \
            .Get = &usart##_N##_get,                                        \
        },                                                                  \
    },



#define USART_INTERFACE(_N, _A)                                             \
    bool usart##_N##_init(usart_cfg_t *ptUsartCFG)                          \
    {                                                                       \
        return usart_init((__usart_t *)&__USART[_N], ptUsartCFG);           \
    }                                                                       \
                                                                            \
    bool usart##_N##_idle(void)                                             \
    {                                                                       \
        return usart_idle((__usart_t *)&__USART[_N]);                       \
    }                                                                       \
                                                                            \
    bool usart##_N##_enable(void)                                           \
    {                                                                       \
        return usart_enable((__usart_t *)&__USART[_N]);                     \
    }                                                                       \
                                                                            \
    bool usart##_N##_disable(void)                                          \
    {                                                                       \
        return usart_disable((__usart_t *)&__USART[_N]);                    \
    }                                                                       \
                                                                            \
    bool usart##_N##_write_byte(uint8_t chByte)                             \
    {                                                                       \
        return usart_write_byte((__usart_t *)&__USART[_N], chByte);         \
    }                                                                       \
                                                                            \
    bool usart##_N##_read_byte(uint8_t *pchByte)                            \
    {                                                                       \
        return usart_read_byte((__usart_t *)&__USART[_N], pchByte);         \
    }                                                                       \
                                                                            \
    bool usart##_N##_set(uint32_t wBaudrate)                                \
    {                                                                       \
        return usart_set((__usart_t *)&__USART[_N], wBaudrate);             \
    }                                                                       \
                                                                            \
    uint32_t usart##_N##_get(void)                                          \
    {                                                                       \
        return usart_get((__usart_t *)&__USART[_N]);                        \
    }                                                                       


#define USART_STATEMENT(_N, _A)                                             \
    extern bool usart##_N##_init(usart_cfg_t *ptUsartCFG);                  \
    extern bool usart##_N##_idle(void);                                     \
    extern bool usart##_N##_enable(void);                                   \
    extern bool usart##_N##_disable(void);                                  \
    extern bool usart##_N##_write_byte(uint8_t chByte);                     \
    extern bool usart##_N##_read_byte(uint8_t *pchByte);                    \
    extern bool usart##_N##_set(uint32_t wBaudrate);                        \
    extern uint32_t usart##_N##_get(void);                                                                
                                                                                      
    
#define this                    (*ptThis)    

#define MODE_LENGTH_MSK        0x0300
#define MODE_LENGTH_SHIFT       8
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


//! \name internal class
//! @{
DEF_CLASS(__usart_t)
    usart_reg_t *const ptREG;
    uint8_t     chIndex;
    //em_ahb_clk_t tAHBClk;             //!< <TBD>
    //em_pclk_no_t tPClk;               //!< <TBD>
    uint8_t chAddressState;
    uint32_t wBaundRate;
END_DEF_CLASS(__usart_t)
//! @}


/*============================ PROTOTYPES ====================================*/

 
/*! \brief idle usart
 *! \param void
 *! \retval true idle succeed
 *! \retval false idle failed
 */
static bool usart_idle(__usart_t *ptUsart);


/*! \brief setup Baudrate
 *! \param  reading object
 *! \retval false setup baudrate false
 *! \retval true setup baudrate true
 */
static bool usart_set(__usart_t *ptUsart, uint32_t wBaudrate);


/*! \brief get Baudrate
 *! \param  none
 *! \retrun Baudrate
 */
static uint32_t usart_get(__usart_t *ptUsart);


/*! \brief initialize usart
 *! \param ptUsartCFG usart configuration object
 *! \retval true initialization succeed
 *! \retval false initialization failed
 */
static bool usart_init(__usart_t *ptUsart, usart_cfg_t *ptUsartCFG);


/*! \brief enable usart
 *! \param void
 *! \retval true enable succeed
 *! \retval false enable failed
 */
static bool usart_enable(__usart_t *ptUsart);


/*! \brief disable usart
 *! \param void
 *! \retval true disable succeed
 *! \retval false disable failed
 */
static bool usart_disable(__usart_t *ptUsart);


/*! \brief write one byte to usart
 *! \param chByte byte of writing
 *! \retval true writing succeed
 *! \retval false writing failed
 */
static bool usart_write_byte(__usart_t *ptUsart, uint8_t chByte);


/*! \brief read byte from usart
 *! \param pchByte reading object
 *! \retval true reading succeed
 *! \retval false reading failed
 */
static bool usart_read_byte(__usart_t *ptUsart, uint8_t *pchByte);


/*----------------------------------------------------------------------------*
 *              USARTx INTERFACE                                              *
 *----------------------------------------------------------------------------*/
MREPEAT(USART_COUNT, USART_STATEMENT, 0)

/*============================ GLOBAL VARIABLES ==============================*/                                               
//! \brief usart object
const usart_t USART[USART_COUNT] = {
    MREPEAT(USART_COUNT, USART_FUNCTION, 0)
};



/*============================ LOCAL VARIABLES ===============================*/
//! \brief internal usart object
static CLASS(__usart_t) __USART[] = {
    MREPEAT(USART_COUNT, __USART_FUNCTION, 0)
};



/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 *              GENERAL INTERFACE                                             *
 *----------------------------------------------------------------------------*/


/*! \brief idle usart
 *! \param void
 *! \retval true idle succeed
 *! \retval false idle failed
 */
static bool usart_idle(__usart_t *ptUsart)
{
    CLASS(__usart_t) *ptThis = (CLASS(__usart_t) *)ptUsart;
    bool bIdle;

    if ((NULL == ptThis) || (NULL == ptUsart)) {
        return false;
    }
    
    SAFE_CLK_CODE(
        //! \note variable 'bIdle' != 1 
        bIdle = (ptThis->ptREG->S1 & UART_S1_TC_MASK);
    )

    return bIdle;
}    


/*! \brief setup Baudrate
 *! \param  reading object
 *! \retval false setup baudrate false
 *! \retval true setup baudrate true
 */
static bool usart_set(__usart_t *ptUsart, uint32_t wBaudrate)
{    
    CLASS(__usart_t) *ptThis = (CLASS(__usart_t) *)ptUsart;

    if ((NULL == ptThis) || (NULL == ptUsart)) {
        return false;
    }

    if (!usart_idle(ptUsart)) {
        return false;
    }
    
    if (!wBaudrate) {
        wBaudrate++;
    }

    SAFE_CLK_CODE(
        do {
            register uint16_t sbr, brfa;
            uint8_t temp;
            uint32_t wClock = 0;

            if (ptThis->ptREG <= &GSP_USART1 ) {
                /* uart0 and uart1 use core clock */
                wClock = USART_GET_CORE_CLK();
            } else {
                /* other uarts use peripheral clock */
                wClock = USART_GET_P_CLK();
            }

            /* Calculate baud settings */
            sbr = (uint16_t)((wClock)/(wBaudrate * 16));
                
            /* Save off the current value of the UARTx_BDH except for the SBR field */
            temp = ptThis->ptREG->BDH & ~(UART_BDH_SBR(0x1F));
            
            ptThis->ptREG->BDH = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
            ptThis->ptREG->BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);
            
            /* Determine if a fractional divider is needed to get closer to the baud rate */
            brfa = (((wClock*2)/wBaudrate) - (sbr * 32));
            
            /* Save off the current value of the UARTx_C4 register except for the BRFA field */
            temp = ptThis->ptREG->C4 & ~(UART_C4_BRFA(0x1F));
            
            ptThis->ptREG->C4 = temp | UART_C4_BRFA(brfa);    

            ptThis->wBaundRate = wBaudrate;
        } while (false);
    )
    return true;
}


/*! \brief get Baudrate
 *! \param  none
 *! \retrun Baudrate
 */
static uint32_t usart_get(__usart_t *ptUsart)
{
    CLASS(__usart_t) *ptThis = (CLASS(__usart_t) *)ptUsart;        
    return ptThis->wBaundRate;
}


/*! \brief initialize usart
 *! \param ptUsartCFG usart configuration object
 *! \retval true initialization succeed
 *! \retval false initialization failed
 */
static bool usart_init(__usart_t *ptUsart, usart_cfg_t *ptUsartCFG)
{
    CLASS(__usart_t) *ptThis = (CLASS(__usart_t) *)ptUsart;

    if ((NULL == ptUsart) || (NULL == ptUsartCFG)) {
        return false;
    }

    SAFE_CLK_CODE (
        do { 
            /* Make sure that the transmitter and receiver are disabled while we 
             * change settings.
             */
            ptThis->ptREG->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
            
            /* Parity Mode */
            ptThis->ptREG->C1 = ptUsartCFG->hwMode; 
            
            /* Set Baudrate */
            usart_set(ptUsart,ptUsartCFG->wBaudrate);


            /* Enable receiver and transmitter */
            ptThis->ptREG->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
              
        } while(0);
    )
    
    return true;
}



/*! \brief enable usart
 *! \param void
 *! \retval true enable succeed
 *! \retval false enable failed
 */
static bool usart_enable(__usart_t *ptUsart)
{
    CLASS(__usart_t) *ptThis = (CLASS(__usart_t) *)ptUsart;
    if ((NULL == ptThis) || (NULL == ptUsart)) {
        return false;
    }

    /* Enable the clock to the selected UART */ 
    SIM_SCGC_UART |= SIM_SCGC_UART0_MASK << (ptThis->chIndex);

    return true;
}    


/*! \brief disable usart
 *! \param void
 *! \retval true disable succeed
 *! \retval false disable failed
 */
static bool usart_disable(__usart_t *ptUsart)
{
    CLASS(__usart_t) *ptThis = (CLASS(__usart_t) *)ptUsart;

    if ((NULL == ptThis) || (NULL == ptUsart)) {
        return false;
    }

    SAFE_CLK_CODE(
        ptThis->wBaundRate = 0;

        /* Disable receiver and transmitter */
        ptThis->ptREG->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );

    )

    /* Disable the clock to the selected UART */ 
    SIM_SCGC_UART &= ~(SIM_SCGC_UART0_MASK << (ptThis->chIndex));

    return true;
}    



/*! \brief write one byte to usart
 *! \param chByte byte of writing
 *! \retval true writing succeed
 *! \retval false writing failed
 */
static bool usart_write_byte(__usart_t *ptUsart, uint8_t chByte)
{
    CLASS(__usart_t) *ptThis = (CLASS(__usart_t) *)ptUsart;
    bool bReturn = false;
    
    do {
        if ((NULL == ptThis) || (NULL == ptUsart)) {
            break;
        }          

        SAFE_CLK_CODE(
            /* check whether space is available in the FIFO */
            if (ptThis->ptREG->S1 & UART_S1_TDRE_MASK) {
                /* Send the character */
                ptThis->ptREG->D = (uint8_t)chByte;          
            }
        )    
        
        bReturn = true;
    } while (false);

    return bReturn;
}    


/*! \brief read byte from usart
 *! \param pchByte reading object
 *! \retval true reading succeed
 *! \retval false reading failed
 */
static bool usart_read_byte(__usart_t *ptUsart, uint8_t *pchByte)
{

    CLASS(__usart_t) *ptThis = (CLASS(__usart_t) *)ptUsart;
    bool bReturn = false;
    
    do {
        if ((NULL == ptThis) || (NULL == ptUsart) || (NULL == pchByte)) {
            break;
        }   

        SAFE_CLK_CODE(
            //! check whether there is a new byte available
            if (ptThis->ptREG->S1 & UART_S1_RDRF_MASK) {
                //! get the 8-bit data from the receiver
                *pchByte = ptThis->ptREG->D;
                bReturn = true;
            }                
        )
    } while (false);

    return bReturn;

}    

/*----------------------------------------------------------------------------*
 *              USARTx INTERFACE                                              *
 *----------------------------------------------------------------------------*/

MREPEAT(USART_COUNT, USART_INTERFACE, 0)
