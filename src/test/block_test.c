#include "block.c"
#include "CuTest.h"

double stepRatio = 0.001;
double accelerationMax = 1;
double speedMax = 1;
int inchMeasurements = 0;

static void initializeMachineTest( CuTest *tc );
static void processBlockTest( CuTest *tc );
static void processGWordTest( CuTest *tc );
static void processPositionTest( CuTest *tc );

static void initializeMachineTest( CuTest* tc ) {
    machine.mode = Linear;
    machine.steps[0] = 50; machine.steps[1] = 50; machine.steps[2] = 50;

    CuAssert( tc, "Machine setup failed.", initializeMachine() );
    CuAssert( tc, "Didn't set Mode.", machine.mode == Rapids );
    CuAssert( tc, "Didn't set X, Y, and Z.", machine.steps[0] == 0 && machine.steps[1] == 0 && machine.steps[2] == 0 );
}

static void processBlockTest( CuTest* tc ) {
    Block block;

    CuAssert( tc, "Good block.", processBlock( "G00X1.234Y342", &block ) );
    CuAssert( tc, "Bad block with unknown word.", !processBlock( "G00X1.234A343", &block ) );
}

static void processGWordTest( CuTest* tc ) {
    Block block;

    CuAssert( tc, "Shouldn't fail on good addresses.", processBlock( "G00G01", &block ) );
    CuAssert( tc, "Should fail on bad address.", !processBlock( "G02", &block ) );
}

static void processPositionTest( CuTest *tc ) {
    Block block;

    initializeMachine();
    CuAssert( tc, "Shouldn't fail on good word.", processWord( "X1.32", &block ) && processWord( "Y3.223", &block ) && processWord( "Z2.55", &block ) );
    CuAssert( tc, "Should have set the number of steps.", block.steps[0] == 1320 && block.steps[1] == 3223 && block.steps[2] == 2550 );

    machine.steps[0] = 100;
    CuAssert( tc, "Shouldn't fail on good word.", processWord( "X1.32", &block ) );
    CuAssert( tc, "Should have set the number of steps, taking into account the machine's position.", block.steps[0] == 1220 );

    CuAssert( tc, "Shouldn't fail on good block.", processBlock( "G91X1.32G90", &block ) );
    CuAssert( tc, "Should have set the number of steps on relative move.", block.steps[0] == 1320 );

    machine.steps[0] = 0;
    CuAssert( tc, "Shouldn't fail on good block.", processBlock( "G20X0.1G21", &block ) );
    CuAssert( tc, "Should have set the number of steps based on inches.", block.steps[0] == 2540 );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, initializeMachineTest );
    SUITE_ADD_TEST( suite, processBlockTest );
    SUITE_ADD_TEST( suite, processGWordTest );
    SUITE_ADD_TEST( suite, processPositionTest );

    return suite;
}
