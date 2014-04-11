#include <math.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "block.h"
#include "cnc.h"
#include "comm.h"
#include "configure.h"

#define FREQUENCY 50000
#define TO_ALG(x) x * UINT32_MAX / FREQUENCY
#define SIGN(x) ( x >= 0 ? 1 : -1 )

static int sendCommand( Command_t *command );

static int processMotorMovement( int32_t steps[] );
static double calculateTotalTime( int32_t steps );
static int sendMotorMovement( MotorMovement_t motorMovements[], char commandType, size_t stepOffset, size_t doubleOffset, int sign );

static void calculateMotorMovement( int32_t maxSteps, int32_t steps, MotorMovement_t *motorMovement );
static int32_t getAccelerationSteps( double acceleration, double speed );

int sendBlock( Block *block ) {
    if( block->steps[0] || block->steps[1] || block->steps[2] ) {
        if( !processMotorMovement( block->steps ) ) {
            return 0;
        }
    }
    return 1;
}

#ifndef TEST
static int sendCommand( Command_t *command ) {
    fprintf( stderr, "X acc/speed: %d, X steps: %d.\n", command->command.accelerating.accelerations[0], command->command.accelerating.steps[0] );

    if( write( fileno( stdout ), command, sizeof( Command_t ) ) == -1 ) {
        fprintf( stderr, "ERROR: could not print to stdout.\n" );
        return 0;
    } else {
        return 1;
    }
}
#endif

static int processMotorMovement( int32_t steps[] ) {
    MotorMovement_t motorMovements[3];
    double totalTime;
    int fastestMotor, i;

    if( abs( steps[0] ) >= abs( steps[1] ) && abs( steps[0] ) >= abs( steps[2] ) ) {
        fastestMotor = 0;
    } else if( abs( steps[1] ) > abs( steps[2] ) ) {
        fastestMotor = 1;
    } else {
        fastestMotor = 2;
    }

    totalTime = calculateTotalTime( steps[fastestMotor] );

    for( i = 0; i < NUM_MOTORS; i++ ) {
        calculateMotorMovement( steps[fastestMotor], steps[i], &motorMovements[i] );
    }


    if( !sendMotorMovement( motorMovements, Accelerating, offsetof( MotorMovement_t, accelerationSteps ), offsetof( MotorMovement_t, acceleration ), 1 )
    || !sendMotorMovement( motorMovements, ConstantSpeed, offsetof( MotorMovement_t, constantSpeedSteps ), offsetof( MotorMovement_t, speed ), 1 )
    || !sendMotorMovement( motorMovements, Accelerating, offsetof( MotorMovement_t, deaccelerationSteps ), offsetof( MotorMovement_t, acceleration ), -1 ) ) {
        return 0;
    }

    return 1;
}

static double calculateTotalTime( int32_t steps ) {
    int32_t accelerationSteps = getAccelerationSteps( accelerationMax, speedMax );
    double accelerationTime, constantSpeedTime;

    if( accelerationSteps * 2 > steps ) {
        accelerationSteps = steps / 2;
    } else {
        accelerationSteps *= 2;
    }
    steps -= 2 * accelerationSteps;
    accelerationTime = 2 * speedMax / accelerationMax / FREQUENCY;
    constantSpeedTime = steps / speedMax;
    return accelerationTime + constantSpeedTime;
}

static int sendMotorMovement( MotorMovement_t motorMovements[], char commandType, size_t stepOffset, size_t doubleOffset, int sign ) {
    Command_t command;
    int i;
    int32_t steps, movement;

    command.commandType = commandType;
    for( i = 0; i < NUM_MOTORS; i++ ) {
        steps = *( int32_t* )( ( char* )&motorMovements[i] + stepOffset );
        movement = TO_ALG( *( double* )( ( char* )&motorMovements[i] + doubleOffset ) );
        command.command.accelerating.steps[i] = steps;
        command.command.accelerating.accelerations[i] = sign * movement;
    }

    if( !sendCommand( &command ) ) {
        return 0;
    }

    return 1;
}

static void calculateMotorMovement( int32_t maxSteps, int32_t steps, MotorMovement_t *motorMovement ) {
    double ratio = ( ( double ) steps ) / abs ( maxSteps );

    motorMovement->speed = speedMax * ratio;
    motorMovement->acceleration = accelerationMax * ratio;
    motorMovement->accelerationSteps = getAccelerationSteps( motorMovement->acceleration, motorMovement->speed );
    motorMovement->deaccelerationSteps = motorMovement->accelerationSteps;
    motorMovement->constantSpeedSteps = abs( steps ) - 2 * motorMovement->accelerationSteps;
}

static int32_t getAccelerationSteps( double acceleration, double speed ) {
    double accelerationInterrupts = speed / acceleration;
    double algorithmAcceleration = TO_ALG( acceleration );
    double accelerationMovement = algorithmAcceleration * accelerationInterrupts * ( accelerationInterrupts + 1 ) / 2;
    return fabs( accelerationMovement ) / UINT32_MAX;
}

