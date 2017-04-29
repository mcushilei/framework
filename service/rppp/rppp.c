/*******************************************************************************
 *  Copyright(C)2017 by Dreistein<mcu_shilei@hotmail.com>                     *
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
#define __RPPP_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\debug\debug.h"
#include "..\queue\queue.h"
#include "..\hash\hash.h"

/*============================ MACROS ========================================*/
#define FRAM_CRC16_POLLY                CRC16_POLY_CCITT

#define RPPP_RCV_HEAD_BYTE              (0xF5)
#define RPPP_SND_HEAD_BYTE              (0xF5)

/*============================ MACROFIED FUNCTIONS ===========================*/
#define RPPP_QUEUE                      queue_t
#define RPPP_QUEUE_INIT()               QUEUE_UINT8_INIT(               \
                                                &rpppRcvQueue,            \
                                                rpppRcvQueueBuffer,         \
                                                ARRAY_LENGTH(rpppRcvQueueBuffer))
#define RPPP_QUEUE_DEINIT()             QUEUE_DEINIT(&rpppRcvQueue)
#define RPPP_ENQUEUE(__BYTE)            QUEUE_ENQUEUE(&rpppRcvQueue, __BYTE)
#define RPPP_DEQUEUE(__PBYTE)           QUEUE_DEQUEUE(&rpppRcvQueue, __PBYTE)
#define RPPP_PEEK_QUEUE(__PBYTE)        QUEUE_PEEK(&rpppRcvQueue, __PBYTE)
#define RPPP_GET_ALL_PEEKED_QUEUE()     QUEUE_GET_ALL_PEEKED(&rpppRcvQueue)
#define RPPP_REST_PEEK_QUEUE()          QUEUE_RESET_PEEK(&rpppRcvQueue)

#define RPPP_CHECKSUM(__BYTE)           crc16_calculator(FRAM_CRC16_POLLY, checksum, __BYTE)

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
extern void rppp_rcv_handle(uint8_t port, uint8_t *pData, uint16_t dataLength);
extern bool rppp_output_byte(uint8_t byte);

/*============================ LOCAL VARIABLES ===============================*/
DEBUG_DEFINE_THIS_FILE("RPPP");

static uint8_t      rpppRcvQueueBuffer[RPPP_PAYLOAD_MAX_SIZE + 4u];
static RPPP_QUEUE   rpppRcvQueue;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

bool rppp_ini(void)
{
    return RPPP_QUEUE_INIT();
}

uint16_t rppp_rcv_fsm(uint8_t byte, uint8_t timeout, uint8_t *pPort, uint8_t **ppData)
{
    static enum {
        RCV_ENQUEUE = 0,
        RCV_PASER,
        RCV_HANDLE,
    } state0 = RCV_ENQUEUE;
    static enum {
        WAIT_FOR_HEAD = 0,
        WAIT_FOR_PORT,
        WAIT_FOR_LENGTH_0,
        WAIT_FOR_LENGTH_1,
        WAIT_FOR_DATA,
        WAIT_FOR_CHECKSUM_0,
        WAIT_FOR_CHECKSUM_1,
    } state1 = WAIT_FOR_HEAD;
    static uint16_t dataLength = 0, writePoint  = 0;
    static uint16_t checksum = 0;
    static uint8_t  port;
    static uint8_t  rpppRcvBuffer[RPPP_PAYLOAD_MAX_SIZE];

    switch (state0) {
        case RCV_ENQUEUE:
            if (timeout) {                      //!< byte timeout.
                RPPP_DEQUEUE(&byte);
                state1 = WAIT_FOR_HEAD;
            } else {                            //!< byte received.
                if (!RPPP_ENQUEUE(byte)) {      //!< queue is full.
                    uint8_t dummy;
                    RPPP_DEQUEUE(&dummy);
                    RPPP_ENQUEUE(byte);
                    state1 = WAIT_FOR_HEAD;
                }
            }
            state0 = RCV_PASER;
            //break;    //!< omitted intentionally.

        case RCV_PASER: {
            bool bReturn = false;
            do {
                if (!RPPP_PEEK_QUEUE(&byte)) {
                    state0 = RCV_ENQUEUE;
                    return 0;
                }

                switch (state1) {
                    case WAIT_FOR_HEAD:
                        if (RPPP_RCV_HEAD_BYTE == byte) {
                            DEBUG_MSG(RPPP_DEBUG, DEBUG_PRINT("Rcv start."););
                            checksum   = 0;
                            dataLength = 0;
                            writePoint = 0;
                            state1 = WAIT_FOR_PORT;
                        } else {
                            RPPP_DEQUEUE(&byte);
                        }
                        break;
                        
                    case WAIT_FOR_PORT:
                        port     = byte;
                        checksum = RPPP_CHECKSUM(byte);
                        state1 = WAIT_FOR_LENGTH_0;
                        break;

                    case WAIT_FOR_LENGTH_0:
                        dataLength = byte << 8;
                        checksum = RPPP_CHECKSUM(byte);
                        state1 = WAIT_FOR_LENGTH_1;
                        break;

                    case WAIT_FOR_LENGTH_1:
                        dataLength |= byte;
                        checksum = RPPP_CHECKSUM(byte);
                        //! validate length
                        if ((dataLength == 0) || (dataLength > RPPP_PAYLOAD_MAX_SIZE)) {
                            RPPP_DEQUEUE(&byte);
                            state1 = WAIT_FOR_HEAD;
                        } else {
                            state1 = WAIT_FOR_DATA;
                        }
                        break;

                    case WAIT_FOR_DATA:
                        rpppRcvBuffer[writePoint] = byte;
                        checksum = RPPP_CHECKSUM(byte);
                        writePoint++;
                        dataLength--;
                        if (0 == dataLength) {
                            state1 = WAIT_FOR_CHECKSUM_0;
                        }
                        break;

                    case WAIT_FOR_CHECKSUM_0:
                        checksum = RPPP_CHECKSUM(byte);
                        state1 = WAIT_FOR_CHECKSUM_1;
                        break;

                    case WAIT_FOR_CHECKSUM_1:
                        checksum = RPPP_CHECKSUM(byte);
                        if (0 == checksum) {
                            DEBUG_MSG(RPPP_DEBUG, DEBUG_PRINT("Rcv cpl."););
                            RPPP_GET_ALL_PEEKED_QUEUE();
                            state0 = RCV_HANDLE;
                            bReturn = true;
                        } else {
                            RPPP_DEQUEUE(&byte);
                        }
                        state1 = WAIT_FOR_HEAD;
                        break;
                }
            } while (!bReturn);
        }
        //break;    //!< omitted intentionally.

        case RCV_HANDLE:
            if (pPort != NULL) {
                *pPort = port;
            }
            if (ppData != NULL) {
                *ppData = rpppRcvBuffer;
            }
            rppp_rcv_handle(port, rpppRcvBuffer, writePoint);
            state0 = RCV_ENQUEUE;
            return writePoint;
    }

    return 0;
}

uint16_t rppp_snd_fsm(uint8_t port, const uint8_t *pData, uint16_t dataLength)
{
    static enum {
        SND_HEAD = 0,
        SND_PORT,
        SND_LENGTH_0,
        SND_LENGTH_1,
        SND_DATA,
        SND_CHECKSUM_0,
        SND_CHECKSUM_1,
    } state = SND_HEAD;
    static uint16_t writePoint = 0;
    static uint16_t checksum = 0;

    switch (state) {
        case SND_HEAD:
            if (0 == dataLength) {
                return 0;
            }
            //DEBUG_MSG(RPPP_DEBUG, DEBUG_PRINT("Snd start:"););
            checksum = 0;
            writePoint = 0;
            if (rppp_output_byte(RPPP_SND_HEAD_BYTE)) {
                state = SND_PORT;
            } else {
                break;
            }
            
        case SND_PORT:
            if (rppp_output_byte(port)) {
                checksum = RPPP_CHECKSUM(port);
                state = SND_LENGTH_0;
            } else {
                break;
            }

        case SND_LENGTH_0:
            if (rppp_output_byte((dataLength >> 8))) {
                checksum = RPPP_CHECKSUM((dataLength >> 8));
                state = SND_LENGTH_1;
            } else {
                break;
            }

        case SND_LENGTH_1:
            if (rppp_output_byte(dataLength)) {
                checksum = RPPP_CHECKSUM(dataLength);
                state = SND_DATA;
            } else {
                break;
            }

        case SND_DATA:
            for (; writePoint < dataLength; writePoint++) {
                if (!rppp_output_byte(pData[writePoint])) {
                    break;
                }
                checksum = RPPP_CHECKSUM(pData[writePoint]);
            }
            if (writePoint >= dataLength) {
                state = SND_CHECKSUM_0;
            } else {
                break;
            }

        case SND_CHECKSUM_0:
            if (rppp_output_byte((checksum >> 8))) {
                state = SND_CHECKSUM_1;
            } else {
                break;
            }

        case SND_CHECKSUM_1:
            if (rppp_output_byte(checksum)) {
                //DEBUG_MSG(RPPP_DEBUG, DEBUG_PRINT("Snd cpl."););
                state = SND_HEAD;
            }
            break;
    }
    
    return writePoint;
}

/* EOF */
