#include "block.h"

int interpretBlock( char *block ) {
    switch( block[0] ) {
        case 'F':
            return 1;
        case 'G':
            return 1;
        case 'I':
            return 1;
        case 'J':
            return 1;
        case 'K':
            return 1;
        case 'X':
            return 1;
        case 'Y':
            return 1;
        case 'Z':
            return 1;
        default:
            return 0;
    }
}
