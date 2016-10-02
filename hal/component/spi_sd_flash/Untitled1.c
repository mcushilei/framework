#include ".\app_cfg.h"


static fsm_rt_t spi_sd_wait_busy(bool *pbRes)
{
    static uint32_t swCnt = 1000;
    static uint8_t schState = 0;
    uint8_t chByte;
    
    switch (schState) {
        case 0:
            swCnt = 1000;
            schState = 1;
            //break;
        case 1:
            if (fsm_rt_ongoing == SD_SPI_READ_BYTE(&chByte)) {
                break;
            }
            
            if (chByte == 0xFF) {
                *pbRes = true;
                schState = 0;
                return fsm_rt_cpl;
            } else{
                if (0 == wCnt) {
                    *pbRes = false;
                    schState = 0;
                    return fsm_rt_cpl;
                }
                wCnt--;
            }
    }

    return fsm_rt_ongoing;    
}

static fsm_rt_t spi_sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t *pchResp, uint32_t len, uint8_t *chRes)
{
    static uint8_t  crc_stop;
    static uint32_t swCnt = 0;
    static uint8_t  schState = 0;
    static uint8_t *spchRes;
    
    switch (schState) {
        case 0: {
            bool bRes;
            //! Wait until the card is ready to receive cmd.
            if (fsm_rt_ongoing == spi_sd_wait_busy(&bRes)) {
                break;
            }
            if (false == bRes) {
                *chRes = 0x81;
                return fsm_rt_cpl;
            }

            //! Calculate CRC.
            cmd |= 0x40;
            crc_stop = SD_CRC7_CALCULATOR(0, cmd);
            crc_stop = SD_CRC7_CALCULATOR(crc_stop, arg >> 24);
            crc_stop = SD_CRC7_CALCULATOR(crc_stop, arg >> 16);
            crc_stop = SD_CRC7_CALCULATOR(crc_stop, arg >> 8);
            crc_stop = SD_CRC7_CALCULATOR(crc_stop, arg);
            crc_stop |= 0x01; //!< CRC7 + Stop bit.
            schState = 1;
            //break;
        }
        case 1:
            if (fsm_rt_ongoing == SD_SPI_WRITE_BYTE(cmd)) {
                break;
            }
            schState = 2;
            //break;
        case 2:
            if (fsm_rt_ongoing == SD_SPI_WRITE_BYTE(arg >> 24)) {
                break;
            }
            schState = 3;
            //break;
        case 3:
            if (fsm_rt_ongoing == SD_SPI_WRITE_BYTE(arg >> 16)) {
                break;
            }
            schState = 4;
            //break;
        case 4:
            if (fsm_rt_ongoing == SD_SPI_WRITE_BYTE(arg >> 8)) {
                break;
            }
            schState = 5;
            //break;
        case 5:
            if (fsm_rt_ongoing == SD_SPI_WRITE_BYTE(arg)) {
                break;
            }
            schState = 6;
            //break;
        case 6:
            if (fsm_rt_ongoing == SD_SPI_WRITE_BYTE(crc_stop)) {/* Valid or dummy CRC plus stop bit */
                break;
            }
            schState = 7;
            swCnt = 8;//! The command response time is 0 to 8 bytes for SDC, 1 to 8 bytes for MMC. */
            //break;
        case 7: {
            uint8_t chByte;
            if (fsm_rt_ongoing == SD_SPI_READ_BYTE(&chByte)) {
                break;
            }
            if (chByte != 0xFF) {
                *chRes = chByte;/* received valid response */
                if (pchResp && len) {
                    swCnt = len;
                    spchRes = pchResp;
                    schState = 8;
                } else {
                    schState = 0;
                    return fsm_rt_cpl;
                }
            } else {
                if (0 == swCnt) {
                    *chRes = 0x82;/* command response time out error */
                    schState = 0;
                    return fsm_rt_cpl;
                } else {
                    swCnt--;
                }
                break;
            }
        }
        case 8:
            if (fsm_rt_ongoing == SD_SPI_READ_BYTE(spchRes)) {
                break;
            }
            spchRes++;
            len--;
            if (0 == len) {
                schState = 0;
                return fsm_rt_cpl;
            }
            break;
    }

    return fsm_rt_ongoing;
}


static fsm_rt_t spi_sd_send_acmd(uint8_t cmd, uint32_t arg, uint8_t *buf, uint32_t len, uint8_t *chRes)
{
    uint8_t wR1;
    static uint8_t schState = 0;
    
    switch (schState) {
        case 0:
            if (fsm_rt_ongoing == spi_sd_send_cmd(SD_CMD_CMD55, 0, NULL, 0, &wR1)) {
                break;
            }
            if (wR1 & (~SD_R1_IDLE_STATE_MSK)) {
               *chRes = wR1;
               return fsm_rt_cpl;
            } else {
                schState = 1;
            }
            break;
        case 1:
            if (fsm_rt_ongoing == spi_sd_send_cmd(cmd, arg, buf, len, &wR1)) {
                break;
            }
            *chRes = wR1;
            schState = 0;
            return fsm_rt_cpl;
    }
    
    return fsm_rt_ongoing;
}


fsm_rt_t spi_sd_erase_block(uint32_t block, uint32_t cnt, bool *bRes)
{
    static uint32_t wStart, wEnd;
    static uint8_t schState = 0;
    static uint32_t swCnt;
    uint8_t chRes;
    
    if (0 == cnt) {
        *bRes = true;
    }

    
    switch (schState) {
        case 0:
            if (0 == cnt) {
                return fsm_rt_cpl;
            }
            wStart = block, wEnd = block + cnt - 1;
    
            //! Convert sector-based address to byte-based address for non SDHC.
            if (SD_V2_0_HC_CARD != s_tCardProperties.chCardType) {
                wStart <<= 9;
                wEnd   <<= 9;
            }
            
            //! The CS signal must be kept low during a transaction.
            SD_SPI_CS_CLR();
            schState = 1;
            //break;
        case 1:
            if (fsm_rt_on_going == spi_sd_send_cmd(SD_CMD_ERASE_WR_BLK_START_ADDR, wStart, NULL, 0, &chRes)) {
                break;
            }
            if (chRes != wR1) {
                SD_SPI_CS_SET();
                *bRes = false;
                schState = 0;
                return fsm_rt_cpl;
            } else {
                schState = 2;
            }
            //break;
        case 2:
            if (fsm_rt_on_going == spi_sd_send_cmd(SD_CMD_ERASE_WR_BLK_END_ADDR, wEnd, NULL, 0, &chRes)) {
                break;
            }
            if (chRes != wR1) {
                SD_SPI_CS_SET();
                *bRes = false;
                schState = 0;
                return fsm_rt_cpl;
            } else {
                schState = 3;
            }
            //break;
        case 3:
            if (fsm_rt_on_going == spi_sd_send_cmd(SD_CMD_ERASE, wEnd, NULL, 0, &chRes)) {
                break;
            }
            if (chRes != wR1) {
                SD_SPI_CS_SET();
                *bRes = false;
                schState = 0;
                return fsm_rt_cpl;
            } else {
                swCnt = 1000;
                schState = 4;
            }
            //break;
        case 4: {
            uint8_t chByte;
            if (fsm_rt_ongoing == SD_SPI_READ_BYTE(&chByte)) {
                brea;
            }
            if (chByte == 0xFF) {
                SD_SPI_CS_SET();
                *bRes = true;
                schState = 0;
                return fsm_rt_cpl;
            } else {
                swCnt--;
                if (0 == swCnt) {
                    SD_SPI_CS_SET();
                    *bRes = false;
                    schState = 0;
                    return fsm_rt_cpl;
                }
            }
            break;
        }
    }
    
    return fsm_rt_ongoing;
}

