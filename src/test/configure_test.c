#include <unistd.h>
#include "configure.c"
#include "CuTest.h"

static void configureTest( CuTest* tc );

static void configureTest( CuTest* tc ) {
    int fd[2];
    FILE *readFile, *writeFile;

    CuAssert( tc, "Couldn't make pipe.", pipe( fd ) == 0 );
    writeFile = fdopen( fd[1], "w" );
    CuAssert( tc, "Didn't open write file.", writeFile != NULL );
    readFile = fdopen( fd[0], "r" );
    CuAssert( tc, "Didn't open read file.", readFile != NULL );

    fprintf( writeFile, "step.ratio=1.03452\n" );
    fclose( writeFile );

    CuAssert( tc, "Configure failed.", configure( readFile ) );
    CuAssert( tc, "Didn't set the stepRatio correctly.", stepRatio == 1.03452 );

    fclose( readFile );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, configureTest );

    return suite;
}
