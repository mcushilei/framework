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
#define __GEOMETRY_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include <math.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct {
    geonum_t    X;
    geonum_t    Y;
} dot2d_t;

typedef struct {
    dot2d_t     A;
    dot2d_t     B;
} segment2d_t;

typedef struct {
    dot2d_t     O;
    geodec_t    R;
} circle2d_t;

typedef struct {
    dot2d_t     LB;
    dot2d_t     RT;
} rectangle2d_t;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
geodec_t dot_distance(dot2d_t *A, dot2d_t *B)
{
    geonum_t x, y;
    x = A->X - B->X;
    x = x * x;
    y = A->Y - B->Y;
    y = y * y;
    return sqrt(x + y);
}

//! left negative, on zero, right positive.
//! bottom negative, on zero, up positive when k of line = 0.
int32_t dot_position_of_line(dot2d_t *d, segment2d_t *l)
{
    geonum_t p;
    dot2d_t *A, *B;

    if (l->A.X <= l->B.X) {
        if (l->A.Y < l->B.Y) {
            A = &(l->A);
            B = &(l->B);
        } else {
            A = &(l->B);
            B = &(l->A);
        }
    } else {        //!< l->A.X > l->B.X
        if (l->B.Y < l->A.Y) {
            A = &(l->B);
            B = &(l->A);
        } else {
            A = &(l->A);
            B = &(l->B);
        }
    }
    p = (A->X - B->X) * (d->Y - A->Y) - (A->Y - B->Y) * (d->X - A->X);
    if (p > 0) {
        return 1;
    } else if (p < 0) {
        return -1;
    } else {
        return 0;
    }
}

static int32_t dot_side_of_line(dot2d_t *d, segment2d_t *l)
{
    geonum_t p;
    dot2d_t *A, *B;

    A = &(l->A);
    B = &(l->B);
    p = (A->X - B->X) * (d->Y - A->Y) - (A->Y - B->Y) * (d->X - A->X);
    if (p > 0) {
        return 1;
    } else if (p < 0) {
        return -1;
    } else {
        return 0;
    }
}

bool is_dot_on_segment(dot2d_t *d, segment2d_t *l)
{
    if (0 != dot_side_of_line(d, l)) {
        return false;
    }

    if (l->A.X < l->B.X) {
        if ((d->X >= l->A.X) && (d->X <= l->B.X)) {
            return true;
        }
    } else {
        if ((d->X >= l->B.X) && (d->X <= l->A.X)) {
            return true;
        }
    }

    return false;
}

bool is_line_horizontal(segment2d_t *l1)
{
    if (l1->A.X == l1->B.X) {
        return true;
    }
    return false;
}

bool is_line_vertical(segment2d_t *l1)
{
    if (l1->A.Y == l1->B.Y) {
        return true;
    }
    return false;
}

bool is_line_parallel(segment2d_t *l1, segment2d_t *l2)
{
    geonum_t p;
    p = (l1->A.X - l1->B.X) * (l2->A.Y - l2->B.Y) - (l1->A.Y - l1->B.Y) * (l2->A.X - l2->B.X);
    if (p == 0) {
        return true;
    }
    return false;
}

//! 0: no intersection.
//! 1: intersection.
//! 2: intersection and at least one segment's terminal on another's.
uint32_t is_segment_intersection(segment2d_t *l1, segment2d_t *l2)
{
    geonum_t p1, p2, p3, p4;

    p1 = dot_side_of_line(&l1->A, l2);
    p2 = dot_side_of_line(&l1->B, l2);
    p3 = dot_side_of_line(&l2->A, l1);
    p4 = dot_side_of_line(&l2->B, l1);

    if ((p1 * p2 > 0) || (p3 * p4 > 0)) {
        return 0;
    }

    if ((p1 == 0) && (p2 == 0)) {
        do {
            if (l2->A.X < l2->B.X) {
                if ((l1->A.X >= l2->A.X) && (l1->A.X <= l2->B.X)) {
                    break;
                }
                if ((l1->B.X >= l2->A.X) && (l1->B.X <= l2->B.X)) {
                    break;
                }
            } else {
                if ((l1->A.X >= l2->B.X) && (l1->A.X <= l2->A.X)) {
                    break;
                }
                if ((l1->B.X >= l2->B.X) && (l1->B.X <= l2->A.X)) {
                    break;
                }
            }
            return 0;
        } while (0);
        return 2;
    }

    if ((p1 * p2 == 0) || (p3 * p4 == 0)) {
        return 2;
    }

    return 1;
}

void get_set_rectangle(dot2d_t *pSet, uint32_t n, rectangle2d_t *pRectangle)
{
    geonum_t max_x = pSet[0].X, min_x = pSet[0].X;
    geonum_t max_y = pSet[0].Y, min_y = pSet[0].Y;
    uint32_t i;

    for (i = 0; i < n; i++) {
        if (max_x < pSet[i].X) {
            max_x = pSet[i].X;
        }
        if (min_x > pSet[i].X) {
            min_x = pSet[i].X;
        }

        if (max_y < pSet[i].Y) {
            max_y = pSet[i].Y;
        }
        if (min_y > pSet[i].Y) {
            min_y = pSet[i].Y;
        }
    }

    pRectangle->LB.X = min_x;
    pRectangle->LB.Y = min_y;
    pRectangle->RT.X = max_x;
    pRectangle->RT.Y = max_y;
}

bool is_dot_in_polygon(dot2d_t *pDot, dot2d_t *pVertex, uint32_t n)
{
    rectangle2d_t retangle;
    segment2d_t   line;
    segment2d_t   segment;
    uint32_t i, cnt = 0;;

    get_set_rectangle(pVertex, n, &retangle);
    if ((pDot->X < retangle.LB.X)
    ||  (pDot->X > retangle.RT.X)
    ||  (pDot->Y < retangle.LB.Y)
    ||  (pDot->Y > retangle.RT.Y)) {
        return false;
    }
    line.A.X = retangle.LB.X;
    line.A.Y = pDot->Y;
    line.B   = *pDot;
    for (i = 0; i != n; i++) {
        segment.A = pVertex[i];
        if ((i + 1) == n) {
            segment.B = pVertex[0];
        } else {
            segment.B = pVertex[i + 1];
        }

        if (is_dot_on_segment(pDot, &segment)) {        //!< those dots on the side are in the polygon.
            return true;
        }

        switch (is_segment_intersection(&line, &segment)) {
            case 1:         //!< intersection.
                cnt++;
                break;

            case 2:         //!< at least one terminal is on the line.
                if ((0 < dot_position_of_line(&segment.A, &line))       //!< onlg those segments above line are valid.
                ||  (0 < dot_position_of_line(&segment.B, &line))) {    //!< ...below or on the line are invalid.
                    cnt++;
                }
                break;

            case 0:         //!< no intersection.
            default:
                break;
        }
    }

    if (cnt & 0x1u) {
        return false;
    } else {
        return true;
    }
}

/* EOF */
