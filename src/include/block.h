#define NUM_MOTORS 3

enum Mode {
    Rapids = 0,
    Linear = 1
};

typedef enum Mode Mode;
typedef struct Block Block;
typedef struct Machine Machine;

struct Machine {
    Mode mode;
    int32_t steps[NUM_MOTORS];
};

struct Block {
    Mode mode;
    int32_t steps[NUM_MOTORS];
};

extern Machine machine;

extern int initializeMachine( void );
extern int processBlock( char *line, Block *block );

