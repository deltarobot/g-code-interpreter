#include <stdio.h>
#include <string.h>
#include "cnc.c"
#include "CuTest.h"

int accelerationMax = 1000;
int speedMax = 2000;

static Command_t expectedCommands[3];
static size_t expectedSizes[3];
static int expectedCommandCount;
static CuTest *ttc;

static void calculateMotorMovementTest( CuTest *tc );
static void accelerationStepsTest( CuTest *tc );
static void processMotorMovementTest( CuTest *tc );

static void calculateMotorMovementTest( CuTest *tc ) {
    MotorMovement_t fastMotor, slowMotor;
    double totalTime, constantSpeedTime;

    fastMotor.acceleration = accelerationMax;
    fastMotor.speed = speedMax;

    calculateMotorMovement( 20000, &fastMotor, 1, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have set the number of steps for the fast motor.",
    fastMotor.accelerationSteps == 2000 && fastMotor.constantSpeedSteps == 16000 && fastMotor.deaccelerationSteps == 2000 );
    CuAssert( tc, "Should have set the time constraints.", totalTime == 12.0 && constantSpeedTime == 8.0 );

    calculateMotorMovement( 10000, &slowMotor, 0, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have calculated the slower motor's speed and acceleration.",
    slowMotor.acceleration = 500 && slowMotor.speed == 1000 );
    CuAssert( tc, "Should have set the number of steps for the slow motor.",
    slowMotor.accelerationSteps == 1000 && slowMotor.constantSpeedSteps == 8000 && slowMotor.deaccelerationSteps == 1000 );

    calculateMotorMovement( 3000, &fastMotor, 1, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have set up for a triangular movement.",
    fastMotor.accelerationSteps == 1500 && fastMotor.constantSpeedSteps == 0 && fastMotor.deaccelerationSteps == 1500 );
    CuAssert( tc, "Timing should reflect triangular movement.", totalTime == 2 * sqrt( 3 ) && constantSpeedTime == 0.0 );

    calculateMotorMovement( 1000, &slowMotor, 0, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have set up for a slower triangular movement.",
    slowMotor.accelerationSteps == 500 && slowMotor.constantSpeedSteps == 0 && slowMotor.deaccelerationSteps == 500 );

    calculateMotorMovement( 0, &slowMotor, 0, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have set up for no movement.",
    slowMotor.accelerationSteps == 0 && slowMotor.constantSpeedSteps == 0 && slowMotor.deaccelerationSteps == 0 );

    calculateMotorMovement( -20000, &fastMotor, 1, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have set the number of steps for the fast motor.",
    fastMotor.accelerationSteps == -2000 && fastMotor.constantSpeedSteps == -16000 && fastMotor.deaccelerationSteps == -2000 );
    CuAssert( tc, "Should have set the time constraints.", totalTime == 12.0 && constantSpeedTime == 8.0 );

    fastMotor.acceleration = accelerationMax;
    fastMotor.speed = 2000;

    calculateMotorMovement( -3000, &fastMotor, 1, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have set up for a negative triangular movement.",
    fastMotor.accelerationSteps == -1500 && fastMotor.constantSpeedSteps == 0 && fastMotor.deaccelerationSteps == -1500 );
    CuAssert( tc, "Timing should reflect triangular movement.", totalTime == 2 * sqrt( 3 ) && constantSpeedTime == 0.0 );
}

static void accelerationStepsTest( CuTest *tc ) {
    CuAssert( tc, "Should return steps performed during acceleration.", accelerationSteps( accelerationMax, 1000 ) == 1000 );
    CuAssert( tc, "Should return steps performed during acceleration.", accelerationSteps( accelerationMax, 975.9 ) == 952 );
}

static void processMotorMovementTest( CuTest *tc ) {
    int32_t steps[NUM_MOTORS] = {20000, 10000, -10000};

    expectedCommandCount = 0;
    expectedCommands[0].commandType = Accelerating;
    expectedCommands[1].commandType = ConstantSpeed;
    expectedCommands[2].commandType = Accelerating;
    expectedCommands[0].command.accelerating.steps[0] = 2000;
    expectedCommands[0].command.accelerating.steps[1] = 1000;
    expectedCommands[0].command.accelerating.steps[2] = 1000;
    expectedCommands[0].command.accelerating.accelerations[0] = 858993;
    expectedCommands[0].command.accelerating.accelerations[1] = 214748;
    expectedCommands[0].command.accelerating.accelerations[2] = -214748;
    expectedCommands[1].command.constantSpeed.steps[0] = 16000;
    expectedCommands[1].command.constantSpeed.steps[1] = 8000;
    expectedCommands[1].command.constantSpeed.steps[2] = 8000;
    expectedCommands[1].command.constantSpeed.speeds[0] = 85899346;
    expectedCommands[1].command.constantSpeed.speeds[1] = 42949673;
    expectedCommands[1].command.constantSpeed.speeds[2] = -42949673;
    expectedCommands[2].command.accelerating.steps[0] = 2000;
    expectedCommands[2].command.accelerating.steps[1] = 1000;
    expectedCommands[2].command.accelerating.steps[2] = 1000;
    expectedCommands[2].command.accelerating.accelerations[0] = -858993;
    expectedCommands[2].command.accelerating.accelerations[1] = -214748;
    expectedCommands[2].command.accelerating.accelerations[2] = 214748;
    expectedSizes[0] = sizeof( Accelerating_t );
    expectedSizes[1] = sizeof( ConstantSpeed_t );
    expectedSizes[2] = sizeof( Accelerating_t );
    ttc = tc;

    processMotorMovement( steps );
    CuAssert( tc, "Should process without segfault?", 1 );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, calculateMotorMovementTest );
    SUITE_ADD_TEST( suite, accelerationStepsTest );
    SUITE_ADD_TEST( suite, processMotorMovementTest );

    return suite;
}

static int sendCommand( Command_t *command, size_t size ) {
    int i;

    CuAssert( ttc, "Should have same command code.", command->commandType == expectedCommands[expectedCommandCount].commandType );
    for( i = 0; i < NUM_MOTORS; i++ ) {
        CuAssert( ttc, "Should have same steps.", command->command.accelerating.steps[i] == expectedCommands[expectedCommandCount].command.accelerating.steps[i] );
        CuAssert( ttc, "Should have same acceleration/speed.", command->command.accelerating.accelerations[i] == expectedCommands[expectedCommandCount].command.accelerating.accelerations[i] );
    }
    CuAssert( ttc, "Should have same size parameter.", size == expectedSizes[expectedCommandCount] );

    expectedCommandCount++;
    return 1;
}

