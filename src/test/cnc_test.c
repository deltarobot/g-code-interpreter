#include <stdio.h>
#include <string.h>
#include "cnc.c"
#include "CuTest.h"

double accelerationMax = 100;
double speedMax = 10000;
int homeDirections[NUM_MOTORS];

static Command_t expectedCommands[3];
static int expectedCommandCount;
static CuTest *ttc;

static void getAccelerationStepsTest( CuTest *tc );
static void calculateTotalTimeTest( CuTest *tc );
static void calculateMotorMovementTest( CuTest *tc );
static void processMotorMovementTest( CuTest *tc );

static void getAccelerationStepsTest( CuTest *tc ) {
    int32_t steps = getAccelerationSteps( accelerationMax, speedMax );
    CuAssert( tc, "Should calculate acceleration steps.", steps == 10 );
}

static void calculateTotalTimeTest( CuTest *tc ) {
    double totalTime = calculateTotalTime( 1000 );
    CuAssert( tc, "Should calculate total run time.", fabs( totalTime - 0.1 ) < 0.00001 );
}

static void calculateMotorMovementTest( CuTest *tc ) {
    MotorMovement_t motorMovement;

    calculateMotorMovement( 1000, 1000, &motorMovement );
    CuAssert( tc, "Should be at max speed.", fabs( motorMovement.speed - speedMax ) < 0.00001 );
    CuAssert( tc, "Should be at max acceleration.", fabs( motorMovement.acceleration - accelerationMax ) < 0.00001 );
    CuAssert( tc, "Should have divided steps.", motorMovement.accelerationSteps == 10 );
    CuAssert( tc, "Should have divided steps.", motorMovement.constantSpeedSteps == 980 );
    CuAssert( tc, "Should have divided steps.", motorMovement.deaccelerationSteps == 10 );

    calculateMotorMovement( 1000, 500, &motorMovement );
    CuAssert( tc, "Should be at half speed.", fabs( motorMovement.speed - speedMax / 2 ) < 0.00001 );
    CuAssert( tc, "Should be at half acceleration.", fabs( motorMovement.acceleration - accelerationMax / 2 ) < 0.00001 );

    calculateMotorMovement( 1000, -1000, &motorMovement );
    CuAssert( tc, "Should be at max negative speed.", fabs( motorMovement.speed + speedMax ) < 0.00001 );
    CuAssert( tc, "Should be at max negative acceleration.", fabs( motorMovement.acceleration + accelerationMax ) < 0.00001 );
    CuAssert( tc, "Should have divided steps.", motorMovement.accelerationSteps == 10 );
    CuAssert( tc, "Should have divided steps.", motorMovement.constantSpeedSteps == 980 );
    CuAssert( tc, "Should have divided steps.", motorMovement.deaccelerationSteps == 10 );
}

static void processMotorMovementTest( CuTest *tc ) {
    int32_t steps[NUM_MOTORS] = {20000, 10000, -10000};

    expectedCommandCount = 0;
    expectedCommands[0].commandType = Accelerating;
    expectedCommands[1].commandType = ConstantSpeed;
    expectedCommands[2].commandType = Accelerating;
    expectedCommands[0].command.accelerating.steps[0] = 10;
    expectedCommands[0].command.accelerating.steps[1] = 5;
    expectedCommands[0].command.accelerating.steps[2] = 5;
    expectedCommands[0].command.accelerating.accelerations[0] = 8589934;
    expectedCommands[0].command.accelerating.accelerations[1] = 4294967;
    expectedCommands[0].command.accelerating.accelerations[2] = -4294967;
    expectedCommands[1].command.constantSpeed.steps[0] = 19980;
    expectedCommands[1].command.constantSpeed.steps[1] = 9990;
    expectedCommands[1].command.constantSpeed.steps[2] = 9990;
    expectedCommands[1].command.constantSpeed.speeds[0] = 858993459;
    expectedCommands[1].command.constantSpeed.speeds[1] = 429496729;
    expectedCommands[1].command.constantSpeed.speeds[2] = -429496729;
    expectedCommands[2].command.accelerating.steps[0] = 10;
    expectedCommands[2].command.accelerating.steps[1] = 5;
    expectedCommands[2].command.accelerating.steps[2] = 5;
    expectedCommands[2].command.accelerating.accelerations[0] = -8589934;
    expectedCommands[2].command.accelerating.accelerations[1] = -4294967;
    expectedCommands[2].command.accelerating.accelerations[2] = 4294967;
    ttc = tc;

    processMotorMovement( steps );
    CuAssert( tc, "Should process without segfault?", 1 );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    /*SUITE_ADD_TEST( suite, getAccelerationStepsTest );
    SUITE_ADD_TEST( suite, calculateTotalTimeTest );
    SUITE_ADD_TEST( suite, calculateMotorMovementTest );
    SUITE_ADD_TEST( suite, processMotorMovementTest );*/

    return suite;
}

#ifdef TEST
static int sendNumberCommands( char numberCommands ) {
    return 1;
}

static int sendCommand( Command_t *command ) {
    int i;

    CuAssert( ttc, "Should have same command code.", command->commandType == expectedCommands[expectedCommandCount].commandType );
    for( i = 0; i < NUM_MOTORS; i++ ) {
        CuAssert( ttc, "Should have same steps.", command->command.accelerating.steps[i] == expectedCommands[expectedCommandCount].command.accelerating.steps[i] );
        CuAssert( ttc, "Should have same acceleration/speed.", command->command.accelerating.accelerations[i] == expectedCommands[expectedCommandCount].command.accelerating.accelerations[i] );
    }

    expectedCommandCount++;
    return 1;
}

static int sendTotalTime( double totalTime ) {
    return 1;
}
#endif

