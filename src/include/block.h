enum Mode {
    Rapids = 0,
    Linear = 1
};

typedef enum Mode Mode;

struct Machine {
    Mode mode;
    double xPosition, yPosition, zPosition;
};

struct Block {
    Mode mode;
    double xPosition, yPosition, zPosition;
    int xSteps, ySteps, zSteps;
};

typedef struct Block Block;
typedef struct Machine Machine;

extern Machine machine;

extern int initializeMachine( void );
extern int processBlock( char * line );