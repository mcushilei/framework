#ifndef __I_IO_RTC_H__
#define __I_IO_RTC_H__

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

#ifndef __REG_MACRO__
#define __REG_MACRO__
#define DEF_REG                     \
        union {                     \
            struct {
    
#define END_DEF_REG(__NAME)         \
            };                      \
            reg32_t Value;          \
        }__NAME;
#endif

//! \name REGISTER DEFIEN
//! @{
/* ---------------------------REGISTER DEFINE BEGIN ------------------------- */
//! \brief RTC data register 
#define RTC_DR_REG                      GSP_RTC.DATA
#define RTC_DR                          RTC_DR_REG

//! \brief RTC match register
#define RTC_MR_REG                      GSP_RTC.MATCH
#define RTC_MR                          RTC_MR_REG

                 
//! \brief RTC load register
#define RTC_LR_REG                      GSP_RTC.LOAD
#define RTC_LR                          RTC_LR_REG

//! \brief RTC control register
#define RTC_CR_REG                      GSP_RTC.CR
#define RTC_CR                          RTC_CR_REG.Value
#define RTC_CR_BIT                      RTC_CR_REG


//! \brief RTC interrupt control set/clear register 
#define RTC_IMSC_REG                    GSP_RTC.IMSC
#define RTC_IMSC                        RTC_IMSC_REG.Value
#define RTC_IMSC_BIT                    RTC_IMSC_REG

//! \brief RTC interrupt status register 
#define RTC_RIS_REG                     GSP_RTC.RIS
#define RTC_RIS                         RTC_RIS_REG.Value
#define RTC_RIS_BIT                     RTC_RIS_REG

//! \brief RTC Masked interrupt status register 
#define RTC_MIS_REG                     GSP_RTC.MIS
#define RTC_MIS                         RTC_MIS_REG.Value
#define RTC_MIS_BIT                     RTC_MIS_REG

//! \brief RTC interrupt clear register 
#define RTC_ICR_REG                     GSP_RTC.ICR
#define RTC_ICR                         RTC_ICR_REG.Value
#define RTC_ICR_BIT                     RTC_ICR_REG

/* ----------------------------REGISTER DEFINE  END-------------------------- */
//! @}

//! \name DEFINE REGISTER OPERATION MACRO
//! @{
/* -----------------------DEFINE REGISTER OP MAC  BETIN---------------------- */

//! \brief RTC control register
#define RTC_CR_RTCSTART_MSK            (1)

//! \brief RTC interrupt control set/clear register 
#define RTC_ICSC_RTCIC_MSK             (1)

//! \brief RTC interrupt status register 
#define RTC_RIS_RTCRIS_MSK             (1)

//! \brief RTC Masked interrupt status register 
#define RTC_MIS_RTCMIS_MSK             (1)

//! \brief RTC interrupt clear register 
#define RTC_ICR_RTCICR_MSK             (1)

/* --------------------------DEFINE REGISTER OP MAC   END-------------------- */
//! @}

/*============================ MACROFIED FUNCTIONS ===========================*/

/*============================ TYPES =========================================*/

//! \name register page type
//! @{
typedef volatile struct {
    //! \brief RTC data register
    reg32_t     DATA;

    //! \brief RTC match register
    reg32_t     MATCH;

    //! \brief RTC load register
    reg32_t     LOAD;

    //! \brief RTC control register
    DEF_REG
        reg32_t RTCSTART                : 1;
        reg32_t SRCSEL                  : 1;
        reg32_t                         :30;
    END_DEF_REG(CR) 

    //! \brief RTC interrupt control set/clear register
    DEF_REG
        reg32_t  RTCIC                  : 1;
        reg32_t                         :31;
    END_DEF_REG(IMSC)

    //! \brief RTC interrupt status register
    DEF_REG
        reg32_t  RTCRIS                 : 1;
        reg32_t                         :31;
    END_DEF_REG(RIS)

    //! \brief RTC Masked interrupt status register
    DEF_REG
        reg32_t  RTCMIS                 : 1;
        reg32_t                         :31;
    END_DEF_REG(MIS)

    //! \brief RTC interrupt clear register
    DEF_REG
        reg32_t  RTCICR                 : 1;
        reg32_t                         :31;
    END_DEF_REG(ICR)

    //! \brief wake-up enable register
    reg32_t     WKUPEN;              
    //! \brief wake-up flag clear register
    reg32_t     WKUPCLR;
    //! \brief wake-up flag status register
    reg32_t     WKUPSTS;
} rtc_reg_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif