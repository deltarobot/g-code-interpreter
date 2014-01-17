#include "block.h"

int interpretBlock( char *block ) {
    switch( block[0] ) {
        case 'F':
            return 1;
            break;
        case 'G':
            return 1;
            break;
        case 'X':
            return 1;
            break;
        case 'Y':
            return 1;
            break;
        case 'Z':
            return 1;
            break;
        default:
            return 0;
    }
}
