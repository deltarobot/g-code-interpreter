enum CommandType {
    ConstantSpeed = 0,
    Accelerating = 1
};

typedef struct ConstantSpeed_t ConstantSpeed_t;
typedef struct Accelerating_t Accelerating_t;
typedef struct Command_t Command_t;

struct ConstantSpeed_t {
    int32_t xSteps, ySteps, zSteps;
    int32_t xSpeed, ySpeed, zSpeed;
};

struct Accelerating_t {
    int32_t xSteps, ySteps, zSteps;
    int32_t xAcceleration, yAcceleration, zAcceleration;
};

struct Command_t {
    char commandType;
    union {
        Accelerating_t accelerating;
        ConstantSpeed_t constantSpeed;
    } command;
};


