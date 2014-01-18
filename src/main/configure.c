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

#define convertToMm(x) x * 25.4

double stepRatio;
int accelerationMax;
int speedMax;

static int processLine( char *line );
static int readProperty( char *propertyName, char *line, ReturnType returnType, ReturnValue *returnValue );

int configure( FILE *file ) {
    char *line = NULL;
    size_t size = 0;

    while( getline( &line, &size, file ) != -1 ) {
        if( !processLine( line ) ) {
            return 0;
        }
    }

    free( line );
    return 1;
}

static int processLine( char *line ) {
    ReturnValue returnValue;

    if( line[0] == '#' ) {

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

