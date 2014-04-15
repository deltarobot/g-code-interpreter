typedef struct MotorMovement_t MotorMovement_t;

struct MotorMovement_t {
    int32_t accelerationSteps, constantSpeedSteps, deaccelerationSteps;
    int32_t algorithmAcceleration, algorithmSpeed;
    double acceleration, speed;
};

extern int sendBlock( Block *block );

