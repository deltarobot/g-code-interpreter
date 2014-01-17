#include "block.c"
#include "CuTest.h"

void interpretBlockTest( CuTest* tc );

void interpretBlockTest( CuTest* tc ) {
    CuAssert( tc, "Supported word.", interpretBlock( "G00" ) );
    CuAssert( tc, "Unsupported word.", !interpretBlock( "H99" ) );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, interpretBlockTest );

    return suite;
}
