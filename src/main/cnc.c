#include <stdint.h>
#include <unistd.h>
#include "block.h"
#include "cnc.h"
#include "comm.h"

#define STDOUT 1

int fd = STDOUT;

int sendBlock( Block *block ) {
    if( block->xSteps || block->ySteps || block->zSteps ) {
        /* TODO: calculate movement and send to motor controller */
        return 1;
    } else {
        return 1;
    }
}

