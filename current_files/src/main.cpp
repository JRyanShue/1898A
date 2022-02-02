
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

// Spin intake x amount
void Intake(int value) {

  intake.spin(vex::directionType::fwd, value, vex::velocityUnits::pct);

}

// Move lift x amount
void Lift(int value) {

  lift.spin(vex::directionType::fwd, value, vex::velocityUnits::pct);

}

void bClawClamp() {

  bClawSet = true;

}

// Automatically set claws based on sensors
void automaticActuation() {

  if (bClawSwitch.value() == 0 && bClawOptical.value(percent) < 70) {

    bClawSet = true;
    
  }

}

// Reset Encoders
void resetDrive() {

  leftD.resetRotation();
  rightD.resetRotation();

}

// Stop Drive
void stopDrive() {

  leftD.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);

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

// Straight drive movement
void straight(float dist, float speed=50.0) {

  // 50 units is one revolution
  float revs = (static_cast<float>(dist)) / 50.0;

  bool reverse = dist < 0;

  resetDrive();
  
  if (reverse) {
    while (leftD.rotation(rotationUnits::rev) > revs || rightD.rotation(rotationUnits::rev) > revs) {
      if (leftD.rotation(rotationUnits::rev) > revs) {
        leftD.spin(vex::directionType::rev, speed, vex::velocityUnits::pct);
      }
      if (rightD.rotation(rotationUnits::rev) > revs) {
        rightD.spin(vex::directionType::rev, speed, vex::velocityUnits::pct);
      }
    }
  } else {
    while (leftD.rotation(rotationUnits::rev) < revs || rightD.rotation(rotationUnits::rev) < revs) {
      if (leftD.rotation(rotationUnits::rev) < revs) {
        leftD.spin(vex::directionType::fwd, speed, vex::velocityUnits::pct);
      }
      if (rightD.rotation(rotationUnits::rev) < revs) {
        rightD.spin(vex::directionType::fwd, speed, vex::velocityUnits::pct);
      }
    }
  }

  //
  stopDrive();
  wait(0.1, sec);
  
}

// Turning drive movement (turn in place)
void turn(float angle, float speed=50.0) {

  // TUNE TURN AMOUNT: HIGHER VALUE IS SHORTER TURN
  float tuneVal = 145.0;

  float revs = (static_cast<float>(angle)) / tuneVal;

  bool reverse = angle < 0;
  // int error;

  resetDrive();

  if (reverse) {
    while (leftD.rotation(rotationUnits::rev) > revs || rightD.rotation(rotationUnits::rev) < -revs) {
      if (leftD.rotation(rotationUnits::rev) > revs) {
        leftD.spin(vex::directionType::rev, speed, vex::velocityUnits::pct);
      }
      if (rightD.rotation(rotationUnits::rev) < -revs) {
        rightD.spin(vex::directionType::fwd, speed, vex::velocityUnits::pct);
      }
    }
  } else {
    while (leftD.rotation(rotationUnits::rev) < revs || rightD.rotation(rotationUnits::rev) > -revs) {
      if (leftD.rotation(rotationUnits::rev) < revs) {
        leftD.spin(vex::directionType::fwd, speed, vex::velocityUnits::pct);
      }
      if (rightD.rotation(rotationUnits::rev) > -revs) {
        rightD.spin(vex::directionType::rev, speed, vex::velocityUnits::pct);
      }
    }
  }

  //
  stopDrive();
  wait(0.1, sec);

}

void fc_flip() {

  fClawSet = !fClawSet;
  setClaws();

}

void bc_flip() {

  bClawSet = !bClawSet;
  setClaws();

}

void auton_sequence() {

  // Drive backwards, grab red mogo, drive forwards
  Intake(0);  // Intake at speed 0/100
  straight(-50);  // Move backwards (negative) for 50 units, or 1 revolution. In reality, will go much further, so just use this as an approximation
  bc_flip();  // "bc" is the claw that sits stationary. This function flips the claw to the other orientation (will close if open, open if closed)
  straight(50);  // Move straight forwards (positive) for 50 units

  // Turn right, push yellow mogo into scoring zone
  turn(90);  // Turn right (positive) for 90 degrees. 90 degrees should be a right turn; tune the value in the function above (noted w/ comment) to change turn amount
  straight(50);

  // Turn left, drop red mogo, push middle yellow mogo into scoring zone
  turn(-120);  // Turn left (negative)
  bc_flip();
  straight(800);

  // Turn right, go backwards and grab red mogo, turn right
  turn(90);
  straight(-400);
  bc_flip();
  turn(90);

  // Push yellow mogo into zone, turn left, drive backwards, release mogo
  straight(800);
  turn(-70);
  straight(-500);
  bc_flip();

  // Turn right, go forward and grab blue mogo, drive backwards, turn left
  

}

void holdDrive(void) {

  lF.setBrake(brakeType::hold);
  lM.setBrake(brakeType::hold);
  lB.setBrake(brakeType::hold);
  rF.setBrake(brakeType::hold);
  rM.setBrake(brakeType::hold);
  rB.setBrake(brakeType::hold);

}

void autonomous(void) {

  // Test PID Loop
  // float kP = 0.7;
  // float kI = 0.2;
  // float kD = 1.5;

  // // Default starting
  // fClawSet = false;
  // bClawSet = false;
  // Intake(100);

  // // lift.resetRotation();
  // int lOrigin = lift.position(deg);
  // int lPos = lift.position(deg) - lOrigin;
  // int i;
  // char buffer[33];
  // while (lPos < 5) {

  //   Brain.Screen.printAt(3, 30, "YO");
  //   lPos = lift.position(deg) - lOrigin;
  //   Intake(-500);

  // }

  // 
  holdDrive();

  // Auton
  // auton_sequence();
  turn(90);
  turn(-90);
  turn(180);
  turn(-180);
  turn(360);

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

  fClawSet = false;
  bClawSet = true;

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

    // Automatic actuation with sensors
    automaticActuation();

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
