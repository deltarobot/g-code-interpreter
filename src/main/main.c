#define _GNU_SOURCE
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

    for( ;; ) {
        if( getline( &line, &size, stdin ) == -1 ) {
            if( !openReadPipe() ) {
                exit( EXIT_FAILURE );
            }
            continue;
        }
        if( !processBlock( line, &block ) ) {
            fprintf( stderr, "Issue with line [%s].\n", line );
        }
        if( !sendBlock( &block ) ) {
            fprintf( stderr, "Issue with processing the block.\n" );
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
        fprintf( stderr, "No pipe to open, using file descriptor %d as normal.\n", fdReplace );
        return 1;
    }

    fd = open( filename, mode );

    if( fd == -1 ) {
        fprintf( stderr, "ERROR: Could not open pipe at %s.\n", filename );
        return 0;
    }

    close( fdReplace );
    dup( fd );
    close( fd );
    if( fdReplace == 1 ) {
        if( fcntl( fdReplace, F_SETPIPE_SZ, 1048576 ) == -1 ) {
            fprintf( stderr, "ERROR: Could not set the pipe size.\n" );
        }
    }

    return 1;
}

