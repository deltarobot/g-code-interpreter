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
static void divideSteps( int32_t steps, int32_t totalAccelerationSteps, MotorMovement_t *motorMovement );
static int32_t accelerationSteps( double acceleration, double speed );

int sendBlock( Block *block ) {
    if( block->xSteps || block->ySteps || block->zSteps ) {
        /* TODO: calculate movement and send to motor controller */
        return 1;
    } else {
        return 1;
    }
}

/* When using for the fastest motor, be sure to set motorMovement->acceleration
   and motorMovement->speed and this function will calculate the timing requirements.
   When using for slower motors, be sure to send in the timing requirements as
   calculated when run for the fastest motor. */
static void calculateMotorMovement( int32_t steps, MotorMovement_t *motorMovement,
char fastestMotor, double *totalTime, double *constantSpeedTime ) {
    int32_t totalAccelerationSteps;

    if( !fastestMotor ) {
        motorMovement->speed = steps * 2 / ( *totalTime + *constantSpeedTime );
        motorMovement->acceleration = 2 * motorMovement->speed / ( *totalTime - *constantSpeedTime );
    }

    totalAccelerationSteps = accelerationSteps( motorMovement->acceleration, motorMovement->speed );

    if( totalAccelerationSteps < steps ) {
        divideSteps( steps, totalAccelerationSteps, motorMovement );
        if( fastestMotor ) {
            *constantSpeedTime = motorMovement->constantSpeedSteps / motorMovement->speed;
            *totalTime = 2 * motorMovement->speed / motorMovement->acceleration + *constantSpeedTime;
        }
    } else {
        divideSteps( steps, steps, motorMovement );
        if( fastestMotor ) {
            *constantSpeedTime = 0.0;
            *totalTime = 2.0 * sqrt( steps / motorMovement->acceleration );
        }
    }
}

static void divideSteps( int32_t steps, int32_t totalAccelerationSteps, MotorMovement_t *motorMovement ) {
    motorMovement->accelerationSteps = ( totalAccelerationSteps + 0.5 ) / 2;
    motorMovement->deaccelerationSteps = totalAccelerationSteps - motorMovement->accelerationSteps;
    motorMovement->constantSpeedSteps = steps - totalAccelerationSteps;
}

static int32_t accelerationSteps( double acceleration, double speed ) {
    return pow( speed, 2 ) / acceleration;
}

