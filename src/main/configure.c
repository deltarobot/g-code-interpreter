#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int accelerationMax = 1;
int speedMax = 1;
int inchMeasurements = 0;

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
    } else if( readProperty( "acceleration.max=", line, Int, &returnValue ) ) {
        if( returnValue.intReturn <= 0 ) {
            fprintf( stderr, "ERROR: acceleration.max must be strictly positive.\n" );
            return 0;
        } else {
            accelerationMax = returnValue.intReturn;
        }
    } else if( readProperty( "speed.max=", line, Int, &returnValue ) ) {
        if( returnValue.intReturn <= 0 ) {
            fprintf( stderr, "ERROR: speed.max must be strictly positive.\n" );
            return 0;
        } else {
            speedMax = returnValue.intReturn;
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

