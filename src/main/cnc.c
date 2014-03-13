#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "block.h"
#include "cnc.h"
#include "comm.h"
#include "configure.h"

#define FREQUENCY 100000
#define ACCELERATION pow( 2, 33 ) / pow( FREQUENCY, 2 )
#define SPEED pow( 2, 32 ) / FREQUENCY

typedef int32_t ( *DoubleConverter )( double );

static int sendCommand( Command_t *command, size_t size );

static int processMotorMovement( int32_t steps[] );
static int sendMotorMovement( MotorMovement_t motorMovements[], char commandType, size_t commandSize, size_t stepOffset,
size_t doubleOffset, DoubleConverter doubleConverter );
static int32_t accelerationConverter( double acceleration );
static int32_t speedConverter( double speed );
static int32_t deaccelerationConverter( double deacceleration );

static void calculateMotorMovement( int32_t steps, MotorMovement_t *motorMovement,
char fastestMotor, double *totalTime, double *constantSpeedTime );
static void divideSteps( int32_t steps, int32_t totalAccelerationSteps, MotorMovement_t *motorMovement );
static int32_t accelerationSteps( double acceleration, double speed );

int sendBlock( Block *block ) {
    if( block->steps[0] || block->steps[1] || block->steps[2] ) {
        if( !processMotorMovement( block->steps ) ) {
            return 0;
        }
    }
    return 1;
}

#ifndef TEST
static int sendCommand( Command_t *command, size_t size ) {
    if( write( fileno( stdout ), &command->command, size ) == -1 ) {
        fprintf( stderr, "ERROR: could not print to stdout.\n" );
        return 0;
    } else {
        return 1;
    }
}
#endif

static int processMotorMovement( int32_t steps[] ) {
    MotorMovement_t motorMovements[3];
    double totalTime, constantSpeedTime;
    int fastestMotor, i;

    if( steps[0] >= steps[1] && steps[0] >= steps[2] ) {
        fastestMotor = 0;
    } else if( steps[1] > steps[2] ) {
        fastestMotor = 1;
    } else {
        fastestMotor = 2;
    }

    motorMovements[fastestMotor].acceleration = accelerationMax;
    motorMovements[fastestMotor].speed = speedMax;
    calculateMotorMovement( steps[fastestMotor], &motorMovements[fastestMotor], 1, &totalTime, &constantSpeedTime );

    for( i = 0; i < 3; i++ ) {
        if( i != fastestMotor ) {
            calculateMotorMovement( steps[i], &motorMovements[i], 0, &totalTime, &constantSpeedTime );
        }
    }

    if( !sendMotorMovement( motorMovements, Accelerating, sizeof( Accelerating_t ), offsetof( MotorMovement_t, accelerationSteps ),
    offsetof( MotorMovement_t, acceleration ), accelerationConverter )
    || !sendMotorMovement( motorMovements, ConstantSpeed, sizeof( ConstantSpeed_t ), offsetof( MotorMovement_t, constantSpeedSteps ),
    offsetof( MotorMovement_t, speed ), speedConverter )
    || !sendMotorMovement( motorMovements, Accelerating, sizeof( Accelerating_t ), offsetof( MotorMovement_t, deaccelerationSteps ),
    offsetof( MotorMovement_t, acceleration ), deaccelerationConverter ) ) {
        return 0;
    }

    return 1;
}

static int sendMotorMovement( MotorMovement_t motorMovements[], char commandType, size_t commandSize, size_t stepOffset,
size_t doubleOffset, DoubleConverter doubleConverter ) {
    Command_t command;
    int i;
    int32_t steps, movement;

    command.commandType = commandType;
    for( i = 0; i < 3; i++ ) {
        steps = *( int32_t* )( ( char* )&motorMovements[i] + stepOffset );
        movement = doubleConverter( *( double* )( ( char* )&motorMovements[i] + doubleOffset ) );
        command.command.accelerating.steps[i] = abs( steps );
        command.command.accelerating.accelerations[i] = movement;
    }

    if( !sendCommand( &command, commandSize ) ) {
        return 0;
    }

    return 1;
}

static int32_t accelerationConverter( double acceleration ) {
    int sign = acceleration < 0 ? -1 : 1;
    return sign * round( pow( acceleration, 2 ) * ACCELERATION );
}

static int32_t speedConverter( double speed ) {
    return round( speed * SPEED );
}

static int32_t deaccelerationConverter( double deacceleration ) {
    return -accelerationConverter( deacceleration );
}

/* When using for the fastest motor, be sure to set motorMovement->acceleration
   and motorMovement->speed and this function will calculate the timing requirements.
   When using for slower motors, be sure to send in the timing requirements as
   calculated when run for the fastest motor. */
static void calculateMotorMovement( int32_t steps, MotorMovement_t *motorMovement,
char fastestMotor, double *totalTime, double *constantSpeedTime ) {
    int32_t totalAccelerationSteps;

    if( fastestMotor && steps < 0 ) {
        motorMovement->acceleration = -motorMovement->acceleration;
        motorMovement->speed = -motorMovement->speed;
    } else if( !fastestMotor ) {
        motorMovement->speed = steps * 2 / ( *totalTime + *constantSpeedTime );
        motorMovement->acceleration = 2 * motorMovement->speed / ( *totalTime - *constantSpeedTime );
    }

    totalAccelerationSteps = accelerationSteps( motorMovement->acceleration, motorMovement->speed );

    if( abs( totalAccelerationSteps ) < abs( steps ) ) {
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
    motorMovement->accelerationSteps = round( totalAccelerationSteps / 2.0 );
    motorMovement->deaccelerationSteps = totalAccelerationSteps - motorMovement->accelerationSteps;
    motorMovement->constantSpeedSteps = steps - totalAccelerationSteps;
}

static int32_t accelerationSteps( double acceleration, double speed ) {
    if( acceleration != 0 ) {
        return pow( speed, 2 ) / acceleration;
    } else {
        return 0;
    }
}

