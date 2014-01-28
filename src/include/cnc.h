typedef struct MotorMovement_t MotorMovement_t;

struct MotorMovement_t {
    int32_t accelerationSteps, constantSpeedSteps, deaccelerationSteps;
    double acceleration, speed;
};

extern int sendBlock( Block *block );

