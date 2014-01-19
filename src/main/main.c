#include <stdlib.h>
#include <stdio.h>
#include "block.h"
#include "configure.h"

static int startupAndConfigure( int argc, char *argv[] );

int main( int argc, char *argv[] ) {
    char *block = NULL;
    size_t size = 0;

    if( !startupAndConfigure( argc, argv ) ) {
        exit( EXIT_FAILURE );
    }

    while( getline( &block, &size, stdin ) != -1 ) {
        if( !interpretBlock( block ) ) {
            exit( EXIT_FAILURE );
        }
    }

    free( block );
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

