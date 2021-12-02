using namespace vex;

extern brain Brain;

// VEXcode devices
extern motor_group MotorGroup2;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );

// Controller

extern controller Controller1;

// Drive motors

extern motor lF;
extern motor lM;
extern motor lB;
extern motor rF;
extern motor rM;
extern motor rB;

extern motor_group leftD;
extern motor_group rightD;

// Mechanism motors

extern motor intake;
extern motor lift;