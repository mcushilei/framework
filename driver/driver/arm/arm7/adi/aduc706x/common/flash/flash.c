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
#include "..\device.h"
#include ".\reg_flash.h"

/*============================ MACROS ========================================*/
#define FLASH_CMD_NULL              (0x00u)
#define FLASH_CMD_SINGLE_READ       (0x01u)
#define FLASH_CMD_SINGLE_WRITE      (0x02u)
#define FLASH_CMD_ERASE_WRITE       (0x03u)
#define FLASH_CMD_SINGLE_VERIFY     (0x04u)
#define FLASH_CMD_SINGLE_ERASE      (0x05u)
#define FLASH_CMD_MASS_ERASE        (0x06u)
#define FLASH_CMD_SIGNATURE         (0x0Bu)
#define FLASH_CMD_PROTECT           (0x0Cu)
#define FLASH_CMD_PING              (0x0Fu)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum {
    FLASH_ERR_NONE              = 0,
    FLASH_ERR_INVALID_ADDR,
    FLASH_ERR_CMD_FAIL,
} em_flash_err_t;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
uint32_t flash_erase_page(uint32_t wPage)
{
    uint32_t wState;
    if ((FLASH_BASE_ADDR + FLASH_SIZE) <= wPage) {
        return FLASH_ERR_INVALID_ADDR;
    }
    wPage &= ~(512u - 1u);
    SAFE_ATOM_CODE(
        FLASH_REG.FEEMOD    = (0u << 9)
                            | (0u << 8)
                            | (0u << 5)
                            | (0u << 4)
                            | (1u << 3)
                            | (0u << 0);
        FLASH_REG.FEEADR = wPage;
        FLASH_REG.FEECON = 0x05;
        do {
            wState = FLASH_REG.FEESTA;
        } while (!(wState & 0x03));
    )
    if (wState & (1u << 1)) {
        return FLASH_ERR_CMD_FAIL;
    }
    return FLASH_ERR_NONE;
}

uint32_t flash_write_data(uint32_t wAddr, uint16_t *phwData, uint32_t wLength)
{
    uint32_t wState;
    if ((FLASH_BASE_ADDR + FLASH_SIZE) <= wAddr) {
        return FLASH_ERR_INVALID_ADDR;
    }

    for (uint32_t i = 0; i < wLength; i++) {
        SAFE_ATOM_CODE(
            FLASH_REG.FEEMOD    = (0u << 9)
                                | (0u << 8)
                                | (0u << 5)
                                | (0u << 4)
                                | (1u << 3)
                                | (0u << 0);
            FLASH_REG.FEEADR = wAddr;
            FLASH_REG.FEEDAT = phwData[i];
            FLASH_REG.FEECON = FLASH_CMD_SINGLE_WRITE;
            do {
                wState = FLASH_REG.FEESTA;
            } while (!(wState & 0x03));
        )
        if (wState & (1u << 1)) {
            return FLASH_ERR_CMD_FAIL;
        }
        wAddr += 2;
    }
    return FLASH_ERR_NONE;
}

/* EOF */
