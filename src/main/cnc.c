#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "block.h"
#include "cnc.h"
#include "comm.h"
#include "configure.h"

#define STDOUT 1

static int32_t accelerationSteps( double constantSpeed, double acceleration );

int sendBlock( Block *block ) {
    if( block->xSteps || block->ySteps || block->zSteps ) {
        /* TODO: calculate movement and send to motor controller */
        return 1;
    } else {
        return 1;
    }
}

static int32_t accelerationSteps( double constantSpeed, double acceleration ) {
    return pow( constantSpeed, 2 ) / acceleration;
}

