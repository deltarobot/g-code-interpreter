enum CommandType {
    ConstantSpeed = 0,
    Accelerating = 1
};

typedef struct Accelerating_t Accelerating_t;
typedef struct ConstantSpeed_t ConstantSpeed_t;
typedef struct Command_t Command_t;

struct Accelerating_t {
    int32_t steps[3];
    int32_t accelerations[3];
};

struct ConstantSpeed_t {
    int32_t steps[3];
    int32_t speeds[3];
};

struct Command_t {
    char commandType;
    union {
        Accelerating_t accelerating;
        ConstantSpeed_t constantSpeed;
    } command;
};


