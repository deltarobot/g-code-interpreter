#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comm.h"
#include "configure.h"

enum ReturnType {
    Double = 0,
    Int = 1
};

union ReturnValue {
    double doubleReturn;
    int intReturn;
};

typedef enum ReturnType ReturnType;
typedef union ReturnValue ReturnValue;

double stepRatio = 1.0;
double accelerationMax = 1;
double speedMax = 1;
int inchMeasurements = 0;
double spindleDutyCycle = 1;
int homeDirections[NUM_MOTORS] = {-1, -1, -1};
int invert[NUM_MOTORS] = {1, 1, 1, 1};

static int processLine( char *line );
static int readProperty( char *propertyName, char *line, ReturnType returnType, ReturnValue *returnValue );

int configure( char *filename ) {
    FILE *configFile;
    char *line = NULL;
    size_t size = 0;

    configFile = fopen( filename, "r" );
    if( configFile == NULL ) {
        fprintf( stderr, "ERROR: could not open file \"%s\".\n", filename );
        return 0;
    }

    while( getline( &line, &size, configFile ) != -1 ) {
        if( !processLine( line ) ) {
            return 0;
        }
    }

    fclose( configFile );
    free( line );
    return 1;
}

static int processLine( char *line ) {
    ReturnValue returnValue;

    if( line[0] == '#' || line[0] == '\n' ) {
        /* This line of the properties file is a comment or a blank line, ignore it. */
    } else if( readProperty( "step.ratio.mm=", line, Double, &returnValue ) ) {
        if( returnValue.doubleReturn <= 0 ) {
            fprintf( stderr, "ERROR: step.ratio must be strictly positive.\n" );
            return 0;
        } else {
            stepRatio = returnValue.doubleReturn;
        }
    } else if( readProperty( "step.ratio.inch=", line, Double, &returnValue ) ) {
        if( returnValue.doubleReturn <= 0 ) {
            fprintf( stderr, "ERROR: step.ratio must be strictly positive.\n" );
            return 0;
        } else {
            stepRatio = convertToMm( returnValue.doubleReturn );
        }
    } else if( readProperty( "acceleration.max=", line, Double, &returnValue ) ) {
        if( returnValue.doubleReturn <= 0 ) {
            fprintf( stderr, "ERROR: acceleration.max must be strictly positive.\n" );
            return 0;
        } else {
            accelerationMax = returnValue.doubleReturn;
        }
    } else if( readProperty( "speed.max=", line, Double, &returnValue ) ) {
        if( returnValue.doubleReturn <= 0 ) {
            fprintf( stderr, "ERROR: speed.max must be strictly positive.\n" );
            return 0;
        } else {
            speedMax = returnValue.doubleReturn;
        }
    } else if( readProperty( "measurement.inch=", line, Int, &returnValue ) ) {
        if( returnValue.intReturn == 0 ) {
            inchMeasurements = 0;
        } else if( returnValue.intReturn == 1 ) {
            inchMeasurements = 1;
        } else {
            fprintf( stderr, "ERROR: measurement.inch must be \"0\" or \"1\".\n" );
            return 0;
        }
    } else if( readProperty( "home.direction.x=", line, Int, &returnValue ) ) {
        homeDirections[0] = sign( returnValue.intReturn );
    } else if( readProperty( "home.direction.y=", line, Int, &returnValue ) ) {
        homeDirections[1] = sign( returnValue.intReturn );
    } else if( readProperty( "home.direction.z=", line, Int, &returnValue ) ) {
        homeDirections[2] = sign( returnValue.intReturn );
    } else if( readProperty( "invert.x=", line, Int, &returnValue ) ) {
        invert[0] = sign( returnValue.intReturn );
    } else if( readProperty( "invert.y=", line, Int, &returnValue ) ) {
        invert[1] = sign( returnValue.intReturn );
    } else if( readProperty( "invert.z=", line, Int, &returnValue ) ) {
        invert[2] = sign( returnValue.intReturn );
    } else if( readProperty( "invert.a=", line, Int, &returnValue ) ) {
        invert[3] = sign( returnValue.intReturn );
    } else if( readProperty( "duty.cycle=", line, Double, &returnValue ) ) {
        if( returnValue.doubleReturn < 0 || returnValue.doubleReturn > 100 ) {
            fprintf( stderr, "ERROR: duty.cycle must be between 0 and 100.\n" );
            return 0;
        } else {
            spindleDutyCycle = returnValue.doubleReturn;
        }
    } else {
        fprintf( stderr, "ERROR: Unknown configuration line: %s", line );
        return 0;
    }
    return 1;
}

static int readProperty( char *propertyName, char *line, ReturnType returnType, ReturnValue *returnValue ) {
    int propertyLength = strlen( propertyName );

    if( strncmp( propertyName, line, propertyLength ) == 0 ) {
        switch( returnType ) {
            case Double:
                returnValue->doubleReturn = strtod( ( line + propertyLength ), NULL );
                return 1;
            case Int:
                returnValue->intReturn = strtol( ( line + propertyLength ), NULL, 10 );
                return 1;
            default:
                return 0;
        }
    } else {
        return 0;
    }
}

