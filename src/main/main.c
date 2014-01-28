#include <stdlib.h>
#include <stdio.h>
#include "block.h"
#include "configure.h"

static int startupAndConfigure( int argc, char *argv[] );

int main( int argc, char *argv[] ) {
    char *line = NULL;
    size_t size = 0;
    Block block;

    if( !startupAndConfigure( argc, argv ) ) {
        exit( EXIT_FAILURE );
    }

    if( !initializeMachine() ) {
        exit( EXIT_FAILURE );
    }

    while( getline( &line, &size, stdin ) != -1 ) {
        if( !processBlock( line, &block ) ) {
            exit( EXIT_FAILURE );
        }
    }

    free( line );
    exit( EXIT_SUCCESS );
}

static int startupAndConfigure( int argc, char *argv[] ) {
    int i;

    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            switch( argv[i][1] ) {
                case 'c':
                    if( !configure( argv[i + 1] ) ) {
                        return 0;
                    }
                    break;
                default:
                    fprintf( stderr, "ERROR: Unknown argument: %s\n", argv[i] );
                    return 0;
            }
        }
    }

    return 1;
}

