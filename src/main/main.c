#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "block.h"
#include "cnc.h"
#include "configure.h"

#define openReadPipe() openPipe( readPipe, 0, O_RDONLY )
#define openWritePipe() openPipe( writePipe, 1, O_WRONLY )

static char *readPipe = NULL;
static char *writePipe = NULL;

static int startupAndConfigure( int argc, char *argv[] );
static int openPipe( char *filename, int fdReplace, mode_t mode );

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
        if( !sendBlock( &block ) ) {
            exit( EXIT_FAILURE );
        }
    }

    free( line );
    exit( EXIT_SUCCESS );
}

static int startupAndConfigure( int argc, char *argv[] ) {
    int i;

    for( i = 1; i < argc; i++ ) {
        if( argv[i][0] == '-' ) {
            switch( argv[i][1] ) {
                case 'c':
                    if( !configure( argv[i + 1] ) ) {
                        return 0;
                    }
                    break;
                case 'r':
                    readPipe = argv[i + 1];
                    break;
                case 'w':
                    writePipe = argv[i + 1];
                    break;
                default:
                    fprintf( stderr, "ERROR: Unknown argument: %s\n", argv[i] );
                    return 0;
            }
        }
    }

    return openReadPipe() && openWritePipe();
}

static int openPipe( char *filename, int fdReplace, mode_t mode ) {
    int fd;

    if( filename == NULL ) {
        printf( "No pipe to open, using file descriptor %d as normal.\n", fdReplace );
        return 1;
    }

    fd = open( filename, mode );

    if( fd == -1 ) {
        fprintf( stderr, "Could not open pipe at %s.\n", filename );
        return 0;
    }

    printf( "Opened pipe at %s.\n", filename );

    close( fdReplace );
    dup( fd );
    close( fd );

    printf( "Duplicated pipe to file descriptor %d.\n", fdReplace );

    return 1;
}

