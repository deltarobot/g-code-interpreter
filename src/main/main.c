#include <stdlib.h>
#include <stdio.h>
#include "block.h"
#include "configure.h"

int main( void ) {
    char *block = NULL;
    size_t size = 0;

    getline( &block, &size, stdin );

    free( block );
    return 0;
}