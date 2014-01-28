#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "block.h"
#include "cnc.h"
#include "comm.h"
#include "configure.h"

#define STDOUT 1

static void calculateMotorMovement( int32_t steps, MotorMovement_t *motorMovement,
char fastestMotor, double *totalTime, double *constantSpeedTime );
static int32_t accelerationSteps( double acceleration, double speed );

int sendBlock( Block *block ) {
    if( block->xSteps || block->ySteps || block->zSteps ) {
        /* TODO: calculate movement and send to motor controller */
        return 1;
    } else {
        return 1;
    }
}

static void calculateMotorMovement( int32_t steps, MotorMovement_t *motorMovement,
char fastestMotor, double *totalTime, double *constantSpeedTime ) {
    int32_t totalAccelerationSteps;

    if( !fastestMotor ) {
        motorMovement->speed = steps * 2 / ( *totalTime + *constantSpeedTime );
        motorMovement->acceleration = 2 * motorMovement->speed / ( *totalTime - *constantSpeedTime );
    }

    totalAccelerationSteps = accelerationSteps( motorMovement->acceleration, motorMovement->speed );

    if( totalAccelerationSteps < steps ) {
        motorMovement->accelerationSteps = ( totalAccelerationSteps + 0.5 ) / 2;
        motorMovement->deaccelerationSteps = totalAccelerationSteps - motorMovement->accelerationSteps;
        motorMovement->constantSpeedSteps = steps - totalAccelerationSteps;
        if( fastestMotor ) {
            *constantSpeedTime = motorMovement->constantSpeedSteps / motorMovement->speed;
            *totalTime = 2 * motorMovement->speed / motorMovement->acceleration + *constantSpeedTime;
        }
    } else {
        motorMovement->accelerationSteps = ( steps + 0.5 ) / 2;
        motorMovement->deaccelerationSteps = steps - motorMovement->accelerationSteps;
        motorMovement->constantSpeedSteps = 0;
        if( fastestMotor ) {
            *constantSpeedTime = 0.0;
            *totalTime = 2.0 * sqrt( steps / motorMovement->acceleration );
        }
    }
}

static int32_t accelerationSteps( double acceleration, double speed ) {
    return pow( speed, 2 ) / acceleration;
}

