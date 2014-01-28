enum Mode {
    Rapids = 0,
    Linear = 1
};

typedef enum Mode Mode;
typedef struct Block Block;
typedef struct Machine Machine;

struct Machine {
    Mode mode;
    int32_t xSteps, ySteps, zSteps;
};

struct Block {
    Mode mode;
    int32_t xSteps, ySteps, zSteps;
};

extern Machine machine;

extern int initializeMachine( void );
extern int processBlock( char *line, Block *block );

