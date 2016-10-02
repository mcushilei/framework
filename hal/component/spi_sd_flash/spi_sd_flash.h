
#ifndef __SPI_SD_H__
#define __SPI_SD_H__

/*============================ INCLUDES ======================================*/
#include ".\spi_sd_flash_port.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool spi_sd_card_detect(void);
extern bool spi_sd_get_card_info(void);
extern bool spi_sd_read_blocks(uint32_t sect, uint8_t *buf, uint32_t cnt);
extern bool spi_sd_write_block(uint32_t sect, const uint8_t *buf, uint32_t cnt);
extern bool spi_sd_erase_block(uint32_t block, uint32_t cnt);

#endif /* __SPI_SD_H__ */
/* EOF */
