#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configure.h"

enum ReturnType {
    Double = 0
};

union ReturnValue {
    double doubleReturn;
};

typedef enum ReturnType ReturnType;
typedef union ReturnValue ReturnValue;

double stepRatio;

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

    return 1;
}

static int processLine( char *line ) {
    ReturnValue returnValue;

    if( line[0] == '#' ) {

    } else if( readProperty( "step.ratio=", line, Double, &returnValue ) ) {
        if( returnValue.doubleReturn <= 0 ) {
            fprintf( stderr, "step.ratio must be strictly positive.\n" );
            return 0;
        } else {
            stepRatio = returnValue.doubleReturn;
        }
    } else {
        fprintf( stderr, "Unknown configuration line: %s", line );
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
            default:
                return 0;
        }
    } else {
        return 0;
    }
}
