#include <math.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "block.h"
#include "cnc.h"
#include "comm.h"
#include "configure.h"

#define FREQUENCY 50000
#define SPEED_CONVERTER(x) ( x * UINT32_MAX / FREQUENCY )
#define ACCELERATION_CONVERTER(x) ( SPEED_CONVERTER( x ) / ( FREQUENCY - 1 ) )

static int sendNumberCommands( char numberCommands );
static int sendCommand( Command_t *command );
static int sendTotalTime( double time );
static int sendData( void *data, size_t size );

static int processMotorMovement( int32_t steps[] );
static double calculateTotalTime( int32_t steps );
static int sendMotorMovement( MotorMovement_t motorMovements[], int32_t commandType, size_t stepOffset, size_t doubleOffset, int sign );
static void calculateMotorMovement( int32_t maxSteps, int32_t steps, MotorMovement_t *motorMovement );
static int32_t getAccelerationSteps( double acceleration, double speed );

static int processHome( void );

int sendBlock( Block *block ) {

    if( block->steps[0] || block->steps[1] || block->steps[2] ) {
        sendNumberCommands( 3 );
        return processMotorMovement( block-> steps );
    } else if( block->home ) {
        return processHome();
    }

    return 1;
}

#ifndef TEST
static int sendNumberCommands( char numberCommands ) {
    return sendData( &numberCommands, 1 );
}

static int sendCommand( Command_t *command ) {
    return sendData( command, sizeof( Command_t ) );
}

static int sendTotalTime( double totalTime ) {
    struct timespec time;

    time.tv_sec = ( long )totalTime;
    time.tv_nsec = ( totalTime - time.tv_sec ) * 1000000000;
    return sendData( &time, sizeof( struct timespec ) );
}

static int sendData( void *data, size_t size ) {
    if( write( fileno( stdout ), data, size ) == -1 ) {
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

    totalTime = calculateTotalTime( abs( steps[fastestMotor] ) );

    for( i = 0; i < NUM_MOTORS; i++ ) {
        calculateMotorMovement( steps[fastestMotor], steps[i], &motorMovements[i] );
        motorMovements[i].acceleration = ACCELERATION_CONVERTER( motorMovements[i].acceleration );
        motorMovements[i].speed = SPEED_CONVERTER( motorMovements[i].speed );
    }


    if( !sendMotorMovement( motorMovements, Accelerating, offsetof( MotorMovement_t, accelerationSteps ), offsetof( MotorMovement_t, acceleration ), 1 )
    || !sendMotorMovement( motorMovements, ConstantSpeed, offsetof( MotorMovement_t, constantSpeedSteps ), offsetof( MotorMovement_t, speed ), 1 )
    || !sendMotorMovement( motorMovements, Accelerating, offsetof( MotorMovement_t, deaccelerationSteps ), offsetof( MotorMovement_t, acceleration ), -1 ) ) {
        return 0;
    }

    return sendTotalTime( totalTime );
}

static double calculateTotalTime( int32_t steps ) {
    int32_t actualAccelerationSteps, accelerationSteps = getAccelerationSteps( accelerationMax, speedMax );
    double accelerationTime, constantSpeedTime;

    if( accelerationSteps * 2 > steps ) {
        actualAccelerationSteps = steps;
    } else {
        actualAccelerationSteps = accelerationSteps * 2;
    }
    steps -= actualAccelerationSteps;
    accelerationTime = speedMax / accelerationMax * ( ( double )actualAccelerationSteps / accelerationSteps );
    constantSpeedTime = steps / speedMax;
    return accelerationTime + constantSpeedTime;
}

static int sendMotorMovement( MotorMovement_t motorMovements[], int32_t commandType, size_t stepOffset, size_t doubleOffset, int sign ) {
    Command_t command;
    int i;
    int32_t steps, movement;

    command.commandType = commandType;
    for( i = 0; i < NUM_MOTORS; i++ ) {
        steps = *( int32_t* )( ( char* )&motorMovements[i] + stepOffset );
        movement = *( double* )( ( char* )&motorMovements[i] + doubleOffset );
        command.command.accelerating.steps[i] = steps;
        command.command.accelerating.accelerations[i] = sign * movement;
    }

    if( !sendCommand( &command ) ) {
        return 0;
    }

    return 1;
}

static void calculateMotorMovement( int32_t maxSteps, int32_t steps, MotorMovement_t *motorMovement ) {
    int32_t accelerationSteps;
    double ratio = ( ( double ) steps ) / abs( maxSteps );

    motorMovement->speed = speedMax * ratio;
    motorMovement->acceleration = accelerationMax * ratio;
    accelerationSteps = getAccelerationSteps( motorMovement->acceleration, motorMovement->speed );
    if( accelerationSteps * 2 > abs( steps ) ) {
        accelerationSteps = abs( steps ) / 2;
    }
    motorMovement->accelerationSteps = accelerationSteps;
    motorMovement->deaccelerationSteps = accelerationSteps;
    motorMovement->constantSpeedSteps = abs( steps ) - 2 * accelerationSteps;
}

static int32_t getAccelerationSteps( double acceleration, double speed ) {
    return 0.5 * speed * speed / fabs( acceleration );
}

static int sendHomeCommand( int lookForNoHome, double accelerationDouble, double speedDouble ) {
    Command_t command;
    int32_t accelerationSteps = getAccelerationSteps( accelerationDouble, speedDouble );
    int32_t acceleration = ACCELERATION_CONVERTER( accelerationDouble );
    int32_t speed = SPEED_CONVERTER( speedDouble );
    int i;

    for( i = 0; i < NUM_MOTORS; i++ ) {
        sendNumberCommands( 3 );

        memset( &command, 0, sizeof( Command_t ) );
        command.commandType = Accelerating;
        command.command.accelerating.steps[i] = accelerationSteps;
        command.command.accelerating.accelerations[i] = acceleration * homeDirections[i];
        sendCommand( &command );

        memset( &command, 0, sizeof( Command_t ) );
        command.commandType = lookForNoHome ? ReverseHome : Home;
        command.command.constantSpeed.steps[i] = INT32_MAX;
        command.command.constantSpeed.speeds[i] = speed * homeDirections[i];
        sendCommand( &command );

        memset( &command, 0, sizeof( Command_t ) );
        command.commandType = Accelerating;
        command.command.accelerating.steps[i] = accelerationSteps;
        command.command.accelerating.accelerations[i] = - acceleration * homeDirections[i];
        sendCommand( &command );

        if( !sendTotalTime( 0 ) ) {
            return 0;
        }
    }

    return 1;
}


static int processHome( void ) {
    sendHomeCommand( 0, accelerationMax, speedMax );
    sendHomeCommand( 1, -accelerationMax, -speedMax );
    sendHomeCommand( 0, accelerationMax, speedMax / 20 );
    return 1;
}
