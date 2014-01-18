#include <unistd.h>
#include "configure.c"
#include "CuTest.h"

static void configureString( CuTest *tc, char *properties, int success );
static void configureMmTest( CuTest *tc );
static void configureInchTest( CuTest *tc );
static void configureCommentsTest( CuTest *tc );
static void configureBadPropertyTest( CuTest *tc );

static void configureString( CuTest *tc, char *properties, int success ) {
    int fd[2];
    FILE *readFile, *writeFile;

    CuAssert( tc, "Couldn't make pipe.", pipe( fd ) == 0 );
    writeFile = fdopen( fd[1], "w" );
    CuAssert( tc, "Didn't open write file.", writeFile != NULL );
    readFile = fdopen( fd[0], "r" );
    CuAssert( tc, "Didn't open read file.", readFile != NULL );

    fprintf( writeFile, properties );
    fclose( writeFile );

    CuAssert( tc, "Configure failed.", configure( readFile ) == success );

    fclose( readFile );
}

static void configureMmTest( CuTest *tc ) {
    configureString( tc, "step.ratio.mm=1.02342\n", 1 );
    CuAssert( tc, "Didn't set the stepRatio correctly.", stepRatio == 1.02342 );
}

static void configureInchTest( CuTest *tc ) {
    configureString( tc, "step.ratio.inch=0.0032\n", 1 );
    CuAssert( tc, "Didn't set the stepRatio correctly.", stepRatio == 0.08128 );
}

static void configureCommentsTest( CuTest *tc ) {
    configureString( tc, "#Some comment\n#Another Comment\n", 1 );
}

static void configureBadPropertyTest( CuTest *tc ) {
    configureString( tc, "unknown.property\n", 0 );
    configureString( tc, "step.ratio.mm=-0.345\n", 0 );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, configureMmTest );
    SUITE_ADD_TEST( suite, configureInchTest );
    SUITE_ADD_TEST( suite, configureCommentsTest );
    SUITE_ADD_TEST( suite, configureBadPropertyTest );

    return suite;
}
