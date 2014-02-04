#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "block.h"
#include "comm.h"
#include "configure.h"

#define handler(char,literal) case char: return process ## literal ## Word( word + 1, block )

Machine machine;

static void cleanupBlock( Block *block );

static int processWord( char *word, Block *block );
static int processGWord( char *address, Block *block );
static int processXWord( char *address, Block *block );
static int processYWord( char *address, Block *block );
static int processZWord( char *address, Block *block );
static void calculateAbsoluteSteps( char *address, Block *block, int32_t oldSteps, int32_t *newSteps );

int initializeMachine( void ) {
    int i;
    /* TODO: home the machine. */

    machine.mode = Rapids;
    machine.absolute = 1;
    for( i = 0; i < NUM_MOTORS; i++ ) {
        machine.steps[i] = 0;
    }
    return 1;
}

int processBlock( char *line, Block *block ) {
    cleanupBlock( block );

    for( ; *line != '\0'; line++ ) {
        if( isupper( ( int )*line ) ) {
            if( !processWord( line, block ) ) {
                return 0;
            }
        }
    }

    return 1;
}

static void cleanupBlock( Block *block ) {
    int i;

    block->mode = machine.mode;
    block->absolute = machine.absolute;
    for( i = 0; i < NUM_MOTORS; i++ ) {
        block->steps[i] = 0;
    }
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
        case 0:
            block->mode = Rapids;
            break;
        case 1:
            block->mode = Linear;
            break;
        case 20:
            inchMeasurements = 1;
            break;
        case 21:
            inchMeasurements = 0;
            break;
        case 90:
            block->absolute = 1;
            break;
        case 91:
            block->absolute = 0;
            break;
        default:
            fprintf( stderr, "ERROR: Unknown address for G word: \"%d\".\n", intAddress );
            return 0;
    }

    return 1;
}

static int processXWord( char *address, Block *block ) {
    calculateAbsoluteSteps( address, block, machine.steps[0], &block->steps[0] );
    return 1;
}

static int processYWord( char *address, Block *block ) {
    calculateAbsoluteSteps( address, block, machine.steps[1], &block->steps[1] );
    return 1;
}

static int processZWord( char *address, Block *block ) {
    calculateAbsoluteSteps( address, block, machine.steps[2], &block->steps[2] );
    return 1;
}

static void calculateAbsoluteSteps( char *address, Block *block ,int32_t oldSteps, int32_t *newSteps ) {
    double position = strtod( address, NULL );
    if( inchMeasurements ) {
        position = convertToMm( position );
    }
    *newSteps = round( position / stepRatio );
    if( block->absolute ) {
        *newSteps -= oldSteps;
    }
}
