#include <stdio.h>
#include <string.h>
#include "cnc.c"
#include "CuTest.h"

int accelerationMax = 1000;
int speedMax = 10000;

static void sendBlockTest( CuTest *tc );
static void calculateMotorMovementTest( CuTest *tc );
static void accelerationStepsTest( CuTest *tc );

static void sendBlockTest( CuTest *tc ) {
    Block block;

    CuAssert( tc, "Should be good to send nothing.", sendBlock( &block ) );
}

static void calculateMotorMovementTest( CuTest *tc ) {
    MotorMovement_t fastMotor, slowMotor;
    double totalTime = 0.0, constantSpeedTime = 0.0;

    fastMotor.acceleration = accelerationMax;
    fastMotor.speed = 2000;

    calculateMotorMovement( 20000, &fastMotor, 1, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have set the number of steps for the fast motor.",
    fastMotor.accelerationSteps == 2000 && fastMotor.constantSpeedSteps == 16000 && fastMotor.deaccelerationSteps == 2000 );
    CuAssert( tc, "Should have set the time constraints.", totalTime == 12.0 && constantSpeedTime == 8.0 );

    calculateMotorMovement( 10000, &slowMotor, 0, &totalTime, &constantSpeedTime );
    CuAssert( tc, "Should have calculated the slower motor's speed and acceleration.",
    slowMotor.acceleration = 500 && slowMotor.speed == 1000 );
}

static void accelerationStepsTest( CuTest *tc ) {
    CuAssert( tc, "Should return steps performed during acceleration.", accelerationSteps( accelerationMax, 1000 ) == 1000 );
    CuAssert( tc, "Should return steps performed during acceleration.", accelerationSteps( accelerationMax, 975.9 ) == 952 );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, sendBlockTest );
    SUITE_ADD_TEST( suite, calculateMotorMovementTest );
    SUITE_ADD_TEST( suite, accelerationStepsTest );

    return suite;
}
