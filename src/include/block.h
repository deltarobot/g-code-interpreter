#define NUM_MOTORS 4

enum Mode {
    Rapids = 0,
    Linear = 1
};

typedef enum Mode Mode;
typedef struct Block Block;
typedef struct Machine Machine;

struct Machine {
    Mode mode;
    char absolute;
    char spindleForwardDirection;
    int32_t steps[NUM_MOTORS];
};

struct Block {
    Mode mode;
    char absolute;
    char home;
    char spindleOn, spindleOff, spindleForwardDirection;
    int32_t steps[NUM_MOTORS];
    char *lcdString;
};

extern int initializeMachine( void );
extern int processBlock( char *line, Block *block );

