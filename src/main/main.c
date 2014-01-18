#include <stdlib.h>
#include <stdio.h>
#include "block.h"
#include "configure.h"

char *configFilename = "config.properties";

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
    return 0;
}

static int startupAndConfigure( int argc, char *argv[] ) {
    FILE *configFile;
    int i;

    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            switch( argv[i][1] ) {
                case 'c':
                    configFilename = argv[i + 1];
                    break;
                default:
                    fprintf( stderr, "ERROR: Unknown argument: %s\n", argv[i] );
                    return 0;
            }
        }
    }

    configFile = fopen( configFilename, "r" );
    if( configFile == NULL ) {
        fprintf( stderr, "ERROR: could not open file %s.\n", configFilename );
        return 0;
    }
    if( !configure( configFile ) ) {
        fclose( configFile );
        return 0;
    }
    fclose( configFile );

    return 1;
}

