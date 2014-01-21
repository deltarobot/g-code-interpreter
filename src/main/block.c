#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "block.h"
#include "configure.h"

#define handler(char,literal) case char: return process ## literal ## Word( word + 1, block )

Machine machine;

static int processWord( char *word, Block *block );
static int processGWord( char *address, Block *block );
static int processXWord( char *address, Block *block );
static int processYWord( char *address, Block *block );
static int processZWord( char *address, Block *block );
static void getPosition( char *address, double oldPosition, double *newPosition, int *steps );

int initializeMachine( void ) {
    /* TODO: home the machine. */
    machine.mode = Rapids;
    machine.xPosition = 0.0;
    machine.yPosition = 0.0;
    machine.zPosition = 0.0;
    return 1;
}

int processBlock( char *line ) {
    Block block;

    for( ; *line != '\0'; line++ ) {
        if( isupper( ( int )*line ) ) {
            if( !processWord( line, &block ) ) {
                return 0;
            }
        }
    }

    return 1;
}

static int processWord( char *word, Block *block ) {
    switch( word[0] ) {
        handler( 'G', G );
        handler( 'X', X );
        handler( 'Y', Y );
        handler( 'Z', Z );
        default:
            fprintf( stderr, "ERROR: Unknown word: \"%c\".\n", word[0] );
            return 0;
    }
}

static int processGWord( char *address, Block *block ) {
    int intAddress = strtol( address, NULL, 10 );

    switch( intAddress ) {
        case Rapids:
            block->mode = Rapids;
            break;
        case Linear:
            block->mode = Linear;
            break;
        default:
            fprintf( stderr, "ERROR: Unknown address for G word: \"%d\".\n", intAddress );
            return 0;
    }

    return 1;
}

static int processXWord( char *address, Block *block ) {
    getPosition( address, machine.xPosition, &block->xPosition, &block->xSteps );
    return 1;
}

static int processYWord( char *address, Block *block ) {
    getPosition( address, machine.yPosition, &block->yPosition, &block->ySteps );
    return 1;
}

static int processZWord( char *address, Block *block ) {
    getPosition( address, machine.zPosition, &block->zPosition, &block->zSteps );
    return 1;
}

static void getPosition( char *address, double oldPosition, double *newPosition, int *steps ) {
    *newPosition = strtod( address, NULL );
    if( inchMeasurements ) {
        *newPosition = convertToMm( *newPosition );
    }
    *steps = ( int )( ( *newPosition - oldPosition ) / stepRatio + 0.5 );
}
