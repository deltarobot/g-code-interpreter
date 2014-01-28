#include <stdio.h>
#include <string.h>
#include "cnc.c"
#include "CuTest.h"

int accelerationMax = 1000;
int speedMax = 10000;

static void sendBlockTest( CuTest *tc );
static void accelerationStepsTest( CuTest *tc );

static void sendBlockTest( CuTest *tc ) {
    Block block;

    CuAssert( tc, "Should be good to send nothing.", sendBlock( &block ) );
}

static void accelerationStepsTest( CuTest *tc ) {
    CuAssert( tc, "Should return steps performed during acceleration.", accelerationSteps( 1000, accelerationMax ) == 1000 );
    CuAssert( tc, "Should return steps performed during acceleration.", accelerationSteps( 975.9, accelerationMax ) == 952 );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, sendBlockTest );
    SUITE_ADD_TEST( suite, accelerationStepsTest );

    return suite;
}
