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


//! \note do not move this pre-processor statement to other places
#define __COMMUNICATION_FRAME_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\..\debug\debug.h"
#include "..\..\queue\queue.h"
#include "..\..\checksum\checksum.h"

/*============================ MACROS ========================================*/
#ifndef FRAME_PAYLOAD_MAX_SIZE
#define FRAME_PAYLOAD_MAX_SIZE          (32u)
#endif

#if ((!defined(FRAME_HEAD_SEGMENT_SIZE)) ||  FRAME_HEAD_SEGMENT_SIZE == 0)
#undef  FRAME_HEAD_SEGMENT_SIZE
#define FRAME_HEAD_SEGMENT_SIZE         (1u)
#endif

#if ((!defined(FRAME_LENGTH_SEGMENT_SIZE)) ||  FRAME_LENGTH_SEGMENT_SIZE == 0)
#undef  FRAME_LENGTH_SEGMENT_SIZE
#define FRAME_LENGTH_SEGMENT_SIZE       (1u)
#endif

#if ((!defined(FRAME_CHECKSUM_SEGMENT_SIZE)) ||  FRAME_CHECKSUM_SEGMENT_SIZE == 0)
#undef  FRAME_CHECKSUM_SEGMENT_SIZE
#define FRAME_CHECKSUM_SEGMENT_SIZE     (1u)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#define FRAME_QUEUE                         queue_t
#define FRAME_QUEUE_INIT()                  QUEUE_UINT8_INIT(               \
                                                &tFrameRcvQueue,            \
                                                chFrameRcvQueueBuf,         \
                                                UBOUND(chFrameRcvQueueBuf))
#define FRAME_QUEUE_DEINIT()                QUEUE_DEINIT(&tFrameRcvQueue)
#define FRAME_ENQUEUE(__BYTE)               QUEUE_ENQUEUE(&tFrameRcvQueue, __BYTE)
#define FRAME_DEQUEUE(__PBYTE)              QUEUE_DEQUEUE(&tFrameRcvQueue, __PBYTE)
#define FRAME_PEEK_QUEUE(__PBYTE)           QUEUE_PEEK(&tFrameRcvQueue, __PBYTE)
#define FRAME_GET_ALL_PEEKED_QUEUE()        QUEUE_GET_ALL_PEEKED(&tFrameRcvQueue)
#define FRAME_REST_PEEK_QUEUE()             QUEUE_RESET_PEEK(&tFrameRcvQueue)

#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
#ifndef FRAM_CRC16_POLLY
#define FRAM_CRC16_POLLY    CRC16_POLY_CCITT
#endif
#define FRAME_CHECKSUM(__BYTE)  crc16_calculator(FRAM_CRC16_POLLY, s_Checksum, __BYTE)
#else
#ifndef FRAM_CRC8_POLLY
#define FRAM_CRC8_POLLY     CRC8_POLY_CCITT
#endif
#define FRAME_CHECKSUM(__BYTE)  crc8_calculator(FRAM_CRC8_POLLY, s_Checksum, __BYTE)
#endif

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
extern bool frame_output_byte(uint8_t chByte);
extern bool frame_poll_byte(uint8_t *pByte, uint8_t *pTimeoutFlag);

/*============================ LOCAL VARIABLES ===============================*/
DEBUG_DEFINE_THIS_FILE("FRAME");

FRAME_QUEUE tFrameRcvQueue;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

bool frame_ini(void)
{
    return FRAME_QUEUE_INIT();
}

fsm_rt_t frame_rcv_fsm(uint8_t *pchData, uint16_t *phwLength)
{
    static enum {
        RCV_PASER = 0,
        RCV_HANDLE,
    } s_tState0 = RCV_PASER;
    static enum {
        WAIT_FOR_HEAD_0 = 0,
#if FRAME_HEAD_SEGMENT_SIZE > 1
        WAIT_FOR_HEAD_1,
#endif
#if FRAME_LENGTH_SEGMENT_SIZE > 1
        WAIT_FOR_LENGTH_0,
#endif
        WAIT_FOR_LENGTH_1,
        WAIT_FOR_DATA,
#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
        WAIT_FOR_CHECKSUM_0,
#endif
        WAIT_FOR_CHECKSUM_1,
    } s_tState1 = WAIT_FOR_HEAD_0;
#if FRAME_LENGTH_SEGMENT_SIZE > 1
    static uint16_t s_DataLength = 0, s_WritePoint  = 0;
#else
    static uint8_t s_DataLength = 0, s_WritePoint  = 0;
#endif
#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
    static uint16_t s_Checksum = 0;
#else
    static uint8_t  s_Checksum = 0;
#endif
    uint8_t chByte;
    uint8_t dummy;

    switch (s_tState0) {
        case RCV_PASER:
        {
            bool bReturn = false;
            uint8_t timeout = 0u;
            
            if (frame_poll_byte(&chByte, &timeout)) {
                if (timeout != 0) {
                    FRAME_DEQUEUE(&dummy);
                    s_tState1 = WAIT_FOR_HEAD_0;
                } else if (!FRAME_ENQUEUE(chByte)) {            //!< byte received
                    FRAME_DEQUEUE(&dummy);
                    FRAME_ENQUEUE(chByte);
                    s_tState1 = WAIT_FOR_HEAD_0;
                }
            }
            
            do {
                if (!FRAME_PEEK_QUEUE(&chByte)) {
                    return FSM_RT_ONGOING;
                }

                switch (s_tState1) {
                    case WAIT_FOR_HEAD_0:
                        if (FRAME_RCV_HEAD_BYTE_0 == chByte) {
                            DEBUG_MSG(FRAME_DEBUG, "Rcv start.");
                            s_Checksum   = 0;
                            s_DataLength = 0;
                            s_WritePoint = 0;
#if FRAME_HEAD_SEGMENT_SIZE > 1
                            s_tState1 = WAIT_FOR_HEAD_1;
#else
#if FRAME_LENGTH_SEGMENT_SIZE > 1
                            s_tState1 = WAIT_FOR_LENGTH_0;
#else
                            s_tState1 = WAIT_FOR_LENGTH_1;
#endif
#endif
                        } else {
                            FRAME_DEQUEUE(&dummy);
                        }
                        break;

#if FRAME_HEAD_SEGMENT_SIZE > 1
                   case WAIT_FOR_HEAD_1:
                        if (FRAME_RCV_HEAD_BYTE_1 == chByte) {
#if FRAME_LENGTH_SEGMENT_SIZE > 1
                            s_tState1 = WAIT_FOR_LENGTH_0;
#else
                            s_tState1 = WAIT_FOR_LENGTH_1;
#endif
                        } else {
                            FRAME_DEQUEUE(&dummy);
                            s_tState1 = WAIT_FOR_HEAD_0;
                        }
                        break;
#endif

#if FRAME_LENGTH_SEGMENT_SIZE > 1
                    case WAIT_FOR_LENGTH_0:
                        s_DataLength = chByte << 8;
                        s_Checksum = FRAME_CHECKSUM(chByte);
                        s_tState1 = WAIT_FOR_LENGTH_1;
                        break;
#endif

                    case WAIT_FOR_LENGTH_1:
                        s_DataLength |= chByte;
                        s_Checksum = FRAME_CHECKSUM(chByte);
                        //! validate length
                        if ((s_DataLength == 0) || (s_DataLength > FRAME_PAYLOAD_MAX_SIZE)) {
                            FRAME_DEQUEUE(&chByte);
                            s_tState1 = WAIT_FOR_HEAD_0;
                        } else {
                            s_tState1 = WAIT_FOR_DATA;
                        }
                        break;

                    case WAIT_FOR_DATA:
                        pchData[s_WritePoint] = chByte;
                        s_Checksum = FRAME_CHECKSUM(chByte);
                        s_WritePoint++;
                        s_DataLength--;
                        if (0 == s_DataLength) {
#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
                            s_tState1 = WAIT_FOR_CHECKSUM_0;
#else
                            s_tState1 = WAIT_FOR_CHECKSUM_1;
#endif
                        }
                        break;

#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
                    case WAIT_FOR_CHECKSUM_0:
                        s_Checksum = FRAME_CHECKSUM(chByte);
                        s_tState1 = WAIT_FOR_CHECKSUM_1;
                        break;
#endif

                    case WAIT_FOR_CHECKSUM_1:
                        s_Checksum = FRAME_CHECKSUM(chByte);
                        s_tState1 = WAIT_FOR_HEAD_0;
                        if (0u != s_Checksum) {
                            FRAME_DEQUEUE(&dummy);
                            break;
                        }
                        DEBUG_MSG(FRAME_DEBUG, "Rcv cpl.");
                        FRAME_GET_ALL_PEEKED_QUEUE();
                        s_tState0 = RCV_HANDLE;
                        bReturn = true;
                        break;
                }
            } while (!bReturn);
        }
        //break;    //!< omitted intentionally.

        case RCV_HANDLE:
            *phwLength = s_WritePoint;
            s_tState0  = RCV_PASER;
            return FSM_RT_CPL;
    }

    return FSM_RT_ONGOING;
}

fsm_rt_t frame_snd_fsm(const uint8_t *pchData, uint16_t hwLength)
{
    static enum {
        SND_HEAD_0 = 0,
#if FRAME_HEAD_SEGMENT_SIZE > 1
        SND_HEAD_1,
#endif
#if FRAME_LENGTH_SEGMENT_SIZE > 1
        SND_LENGTH_0,
#endif
        SND_LENGTH_1,
        SND_DATA,
#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
        SND_CHECKSUM_0,
#endif
        SND_CHECKSUM_1,
    } s_tState = SND_HEAD_0;
#if FRAME_LENGTH_SEGMENT_SIZE > 1
    static uint16_t  s_WritePoint = 0;
#else
    static uint8_t   s_WritePoint = 0;
#endif
#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
    static uint16_t s_Checksum = 0;
#else
    static uint8_t  s_Checksum = 0;
#endif

    switch (s_tState) {
        case SND_HEAD_0:
            if (0 == hwLength) {
                return FSM_RT_CPL;
            }
            DEBUG_MSG(FRAME_DEBUG, "Snd start:");
            s_Checksum = 0;
            s_WritePoint  = 0;
            if (frame_output_byte(FRAME_SND_HEAD_BYTE_0)) {
#if FRAME_HEAD_SEGMENT_SIZE > 1
                s_tState = SND_HEAD_1;
#else
#if FRAME_LENGTH_SEGMENT_SIZE > 1
                s_tState = SND_LENGTH_0;
#else
                s_tState = SND_LENGTH_1;
#endif
#endif
            }
            break;

#if FRAME_HEAD_SEGMENT_SIZE > 1
        case SND_HEAD_1:
            if (frame_output_byte(FRAME_SND_HEAD_BYTE_1)) {
#if FRAME_LENGTH_SEGMENT_SIZE > 1
                s_tState = SND_LENGTH_0;
#else
                s_tState = SND_LENGTH_1;
#endif
            }
            break;
#endif

#if FRAME_LENGTH_SEGMENT_SIZE > 1
        case SND_LENGTH_0:
            if (frame_output_byte((uint8_t)(hwLength >> 8))) {
                s_Checksum = FRAME_CHECKSUM((uint8_t)(hwLength >> 8));
                s_tState = SND_LENGTH_1;
            }
            break;
#endif

        case SND_LENGTH_1:
            if (frame_output_byte((uint8_t)hwLength)) {
                s_Checksum = FRAME_CHECKSUM((uint8_t)hwLength);
                s_tState = SND_DATA;
            }
            break;

        case SND_DATA:
            if (frame_output_byte(pchData[s_WritePoint])) {
                s_Checksum = FRAME_CHECKSUM(pchData[s_WritePoint]);
                s_WritePoint++;
                if (s_WritePoint >= hwLength) {
#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
                    s_tState = SND_CHECKSUM_0;
#else
                    s_tState = SND_CHECKSUM_1;
#endif
                }
            }
            break;

#if FRAME_CHECKSUM_SEGMENT_SIZE > 1
        case SND_CHECKSUM_0:
            if (frame_output_byte((uint8_t)(s_Checksum >> 8))) {
                s_tState = SND_CHECKSUM_1;
            }
            break;
#endif

        case SND_CHECKSUM_1:
            if (frame_output_byte((uint8_t)s_Checksum)) {
                DEBUG_MSG(FRAME_DEBUG, "Snd cpl.");
                s_tState = SND_HEAD_0;
                return FSM_RT_CPL;
            }
            break;
    }
    
    return FSM_RT_ONGOING;
}

/* EOF */
