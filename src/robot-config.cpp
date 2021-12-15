
// Must include this to use vexcode API
#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen and other utilities
brain Brain;

void vexcodeInit(void) {
  
}

// Ports

pneumatics fClaw = pneumatics(Brain.ThreeWirePort.A);
pneumatics bClaw = pneumatics(Brain.ThreeWirePort.B);

// Controller

controller Controller1 = controller(primary);

// Drive motors

motor lF = motor(PORT7, ratio18_1, true);
motor lM = motor(PORT2, ratio18_1, false);
motor lB = motor(PORT3, ratio18_1, true);
motor rF = motor(PORT4, ratio18_1, false);
motor rM = motor(PORT5, ratio18_1, true);
motor rB = motor(PORT6, ratio18_1, false);

// Set motor group

motor_group leftD = motor_group(lF, lM, lB);
motor_group rightD = motor_group(rF, rM, rB);

// Mechanism motors

motor intake = motor(PORT9, ratio6_1, true);
motor lift = motor(PORT10, ratio36_1, false);
