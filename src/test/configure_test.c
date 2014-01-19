#include "configure.c"
#include "CuTest.h"

static void configureString( CuTest *tc, char *properties, int success );
static void configureMmTest( CuTest *tc );
static void configureInchTest( CuTest *tc );
static void configureAccelerationTest( CuTest *tc );
static void configureSpeedTest( CuTest *tc );
static void configureCommentsTest( CuTest *tc );
static void configureBadPropertyTest( CuTest *tc );

static void configureString( CuTest *tc, char *properties, int success ) {
    char *filename = "temp.properties";
    FILE *file = NULL;

    file = fopen( filename, "w" );
    CuAssert( tc, "Didn't open the temp file.", file != NULL );
    fprintf( file, properties );
    fclose( file );

    CuAssert( tc, "Configure failed.", configure( filename ) == success );

    CuAssert( tc, "Didn't delete file successfully.", remove( filename ) != -1 );
}

static void configureMmTest( CuTest *tc ) {
    configureString( tc, "step.ratio.mm=1.02342\n", 1 );
    CuAssert( tc, "Didn't set the stepRatio correctly.", stepRatio == 1.02342 );
}

static void configureInchTest( CuTest *tc ) {
    configureString( tc, "step.ratio.inch=0.0032\n", 1 );
    CuAssert( tc, "Didn't set the stepRatio correctly.", stepRatio == 0.08128 );
}

static void configureAccelerationTest( CuTest *tc ) {
    configureString( tc, "acceleration.max=500\n", 1 );
    CuAssert( tc, "Didn't set the accelerationMax correctly.", accelerationMax == 500 );
}

static void configureSpeedTest( CuTest *tc ) {
    configureString( tc, "speed.max=10000", 1 );
    CuAssert( tc, "Didn't set the speedMax correctly.", speedMax == 10000 );
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
    SUITE_ADD_TEST( suite, configureAccelerationTest );
    SUITE_ADD_TEST( suite, configureSpeedTest );
    SUITE_ADD_TEST( suite, configureCommentsTest );
    SUITE_ADD_TEST( suite, configureBadPropertyTest );

    return suite;
}
