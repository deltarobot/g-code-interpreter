#include "cnc.c"
#include "CuTest.h"

static void sendBlockTest( CuTest *tc ) {
    Block block;
    CuAssert( tc, "Should be good to send nothing.", sendBlock( &block ) );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, sendBlockTest );

    return suite;
}
