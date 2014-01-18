#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configure.h"

double stepRatio;

static int processLine( char *line );

static int processLine( char *line ) {
    const char *stepRatioString = "step.ratio=";
    const int stepRatioLength = strlen( stepRatioString );

    if( line[0] == '#' ) {

    } else if( strncmp( stepRatioString, line, stepRatioLength ) == 0 ) {
        stepRatio = strtod( ( line + stepRatioLength ), NULL );
        if( stepRatio <= 0 ) {
            fprintf( stderr, "step.ratio must be strictly positive.\n" );
            return 0;
        }
    } else {
        fprintf( stderr, "Unknown configuration line: %s", line );
        return 0;
    }
    return 1;
}

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