#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void) {
  // Nothing to initialize
}

// Ports
#define FCLAW 'A'
#define BCLAW 'B'

// Controller

controller Controller1 = controller(primary);

// Drive motors

motor lF = motor(PORT7, ratio18_1, true);
motor lM = motor(PORT2, ratio18_1, false);
motor lB = motor(PORT3, ratio18_1, true);
motor rF = motor(PORT4, ratio18_1, false);
motor rM = motor(PORT5, ratio18_1, true);
motor rB = motor(PORT6, ratio18_1, false);

motor_group leftD = motor_group(lF, lM, lB);
motor_group rightD = motor_group(rF, rM, rB);

// Mechanism motors

motor intake = motor(PORT9, ratio6_1, true);
motor lift = motor(PORT10, ratio36_1, false);
