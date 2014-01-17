#include <stdlib.h>
#include <stdio.h>
#include "block.h"

int main( void ) {
    char *block;
    size_t size = 100;
    block = malloc( size );

    getline( &block, &size, stdin );

    free( block );
    return 0;
}