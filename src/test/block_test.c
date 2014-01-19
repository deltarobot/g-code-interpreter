#include "block.c"
#include "CuTest.h"

double stepRatio = 0.001;
int accelerationMax = 1;
int speedMax = 1;
int inchMeasurements = 0;

static void initializeMachineTest( CuTest *tc );
static void processBlockTest( CuTest *tc );
static void processGWordTest( CuTest *tc );
static void processPositionTest( CuTest *tc );

static void initializeMachineTest( CuTest* tc ) {
    machine.mode = Linear;
    machine.xPosition = 1.0; machine.yPosition = 1.0; machine.zPosition = 1.0;

    CuAssert( tc, "Machine setup failed.", initializeMachine() );
    CuAssert( tc, "Didn't set Mode.", machine.mode == Rapids );
    CuAssert( tc, "Didn't set X, Y, and Z.", machine.xPosition == 0.0 && machine.yPosition == 0.0 && machine.zPosition == 0.0 );
}

static void processBlockTest( CuTest* tc ) {
    CuAssert( tc, "Good block.", processBlock( "G00X1.234Y342" ) );
    CuAssert( tc, "Bad block with unknown word.", !processBlock( "G00X1.234A343" ) );
}

static void processGWordTest( CuTest* tc ) {
    CuAssert( tc, "Shouldn't fail on good addresses.", processBlock( "G00G01" ) );
    CuAssert( tc, "Should fail on bad address.", !processBlock( "G02" ) );
}

static void processPositionTest( CuTest *tc ) {
    Block block;

    initializeMachine();
    CuAssert( tc, "Shouldn't fail on good word.", processWord( "X1.32", &block ) && processWord( "Y3.223", &block ) && processWord( "Z2.55", &block ) );
    CuAssert( tc, "Should have set the positions.", block.xPosition == 1.32 && block.yPosition == 3.223 && block.zPosition == 2.55 );
    CuAssert( tc, "Should have set the number of steps", block.xSteps == 1320 && block.ySteps == 3223 && block.zSteps == 2550 );

}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, initializeMachineTest );
    SUITE_ADD_TEST( suite, processBlockTest );
    SUITE_ADD_TEST( suite, processGWordTest );
    SUITE_ADD_TEST( suite, processPositionTest );

    return suite;
}
