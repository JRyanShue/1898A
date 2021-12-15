
// Must include this to use vexcode API
#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;

// Global variables
bool fClawSet = true;
bool bClawSet = true;

void pre_auton(void) {

  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

}

void autonomous(void) {

}

// Spin intake x amount
void Intake(int value) {

  intake.spin(vex::directionType::fwd, value, vex::velocityUnits::pct);

}

// Move lift x amount
void Lift(int value) {

  lift.spin(vex::directionType::fwd, value, vex::velocityUnits::pct);

}

// Set front and back claws
void setClaws() {

  if (fClawSet) {
    fClaw.open();
  } else {
    fClaw.close();
  }

  if (bClawSet) {
    bClaw.open();
  } else {
    bClaw.close();
  }

}

void usercontrol(void) {

  // Wait time between cycles of the main loop (ms)
  int cycleTime = 20;

  // Set intake to be default on
  bool intakeOn = true;

  lift.setBrake(brakeType::hold);

  // Cooldowns for pistons so that they don't oscillate back and forth when pressed
	int fCoolDown = 0;
	int bCoolDown = 0;

  // Must actuation button disabled for this amount of time after pressing
  // Makes it so that the piston doesn't shoot back and forth in the time of pressing the button
  int coolDownThreshold = 200;

  // Main loop
  while (1) {

    // Increment cooldowns
    fCoolDown += cycleTime;
		bCoolDown += cycleTime;
    
    // Set tank drive
    leftD.spin(vex::directionType::fwd, Controller1.Axis3.value(), vex::velocityUnits::pct);
    rightD.spin(vex::directionType::fwd, Controller1.Axis2.value(), vex::velocityUnits::pct);

    // Intake

    // Special intake configuration: disable intake or flip direction
    // Assumption is that we want the intake to run by default for convenience picking up rings
    if (Controller1.ButtonX.pressing()) {
			intakeOn = !intakeOn;
		}
		if (intakeOn) {
			Intake(100);
		} else {
			Intake(0);
		}
		if (Controller1.ButtonB.pressing()) {
			Intake(-100);
		}

		// Lift

    // Set to hold (continuous voltage will be sent to hold motor position)
    lift.setBrake(hold);

    // Up, down, or don't move
		if (Controller1.ButtonR1.pressing()) {
			Lift(100);
		} else if (Controller1.ButtonR2.pressing()) {
			Lift(-100);
		} else {
			Lift(0);
		}

    // Claws (pneumatics)

    // Update claws

    // Front claw
		if (Controller1.ButtonL2.pressing() && fCoolDown > coolDownThreshold) {
			fClawSet = !fClawSet;
			fCoolDown = 0;
		}

    // Back claw
		if (Controller1.ButtonL1.pressing() && bCoolDown > coolDownThreshold) {
			bClawSet = !bClawSet;
			bCoolDown = 0;
		}

    // Physically actuate claws based on determined values
		setClaws();    
    
    wait(cycleTime, msec); 
  }
}


// Main function (called on start)
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
