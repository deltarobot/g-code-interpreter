#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "block.h"
#include "cnc.h"
#include "comm.h"
#include "configure.h"

#define STDOUT 1

static void calculateMotorMovement( uint32_t steps, MotorMovement_t *motorMovement, double *totalTime, double *constantSpeedTime );
static int32_t accelerationSteps( double acceleration, double speed );

int sendBlock( Block *block ) {
    if( block->xSteps || block->ySteps || block->zSteps ) {
        /* TODO: calculate movement and send to motor controller */
        return 1;
    } else {
        return 1;
    }
}

static void calculateMotorMovement( uint32_t steps, MotorMovement_t *motorMovement,
double *totalTime, double *constantSpeedTime ) {
    int32_t totalAccelerationSteps = accelerationSteps( motorMovement->acceleration, motorMovement->speed );
    motorMovement->accelerationSteps = ( totalAccelerationSteps + 0.5 ) / 2;
    motorMovement->deaccelerationSteps = totalAccelerationSteps - motorMovement->accelerationSteps;
    motorMovement->constantSpeedSteps = steps - totalAccelerationSteps;
    *constantSpeedTime = motorMovement->constantSpeedSteps / motorMovement->speed;
    *totalTime = 2 * motorMovement->speed / motorMovement->acceleration + *constantSpeedTime;
}

static int32_t accelerationSteps( double acceleration, double speed ) {
    return pow( speed, 2 ) / acceleration;
}

