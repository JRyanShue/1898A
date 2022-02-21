
// Must include this to use vexcode API
#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;

// Global variables
bool fClawSet = true;
bool bClawSet = true;

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

void pre_auton(void) {

  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // Set both claws to down pos
  fClawSet = true;
  bClawSet = true;
  setClaws();

}

// Spin intake x amount
void Intake(float value) {

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

// Straight drive movement
void straight(float dist, float speed=50.0) {

  // PID constants
  float kP = 3.0; //until goes past then little back
  float kI = 0.0; //add until no steady state error so you hit exact every time
  float kD = 0.0; //increase until how far it is when it stops decreases 

  float error = 0.0;
  float integral = 0.0;
  float derivative = 0.0;

  float p;
  float i;
  float d;

  float dT = 20.0;  // ms between update cycles

  // 50 units is one revolution
  float revs = (static_cast<float>(dist)) / 50.0;

  // Initialize previous error
  float prev_error = revs;

  bool reverse = dist < 0;

  float current_pos;

  resetDrive();

  // Set left side voltage
  // For now, assume forward
  while (true) {

    float time_elapsed = 0;

    current_pos = leftD.rotation(rotationUnits::rev);  // current position

    error = revs - current_pos;  // distance to target
    if (error < 0.01 && time_elapsed > 2000) {  // End move if error is small enough
      break;
    }
    integral += error * dT;  // accumulated error over time to counteract lowering in error
    derivative = (error - prev_error)/dT;  // difference in error from last cycle to this -- this value is negative!

    // Three variables that factor into voltage setting
    p = kP * error;
    i = kI * integral;
    d = kD * derivative;

    // Set drive (both sides for now)
    leftD.spin(vex::directionType::fwd, p + i + d, vex::velocityUnits::pct);
    rightD.spin(vex::directionType::fwd, p + i + d, vex::velocityUnits::pct);

    prev_error = error;

    // Wait a constant delay
    wait(dT, sec);
    time_elapsed += dT;

  }
  
  
  // if (reverse) {
  //   while (leftD.rotation(rotationUnits::rev) > revs || rightD.rotation(rotationUnits::rev) > revs) {
  //     if (leftD.rotation(rotationUnits::rev) > revs) {
  //       leftD.spin(vex::directionType::rev, speed, vex::velocityUnits::pct);
  //     }
  //     if (rightD.rotation(rotationUnits::rev) > revs) {
  //       rightD.spin(vex::directionType::rev, speed, vex::velocityUnits::pct);
  //     }
  //   }
  // } else {
  //   while (leftD.rotation(rotationUnits::rev) < revs || rightD.rotation(rotationUnits::rev) < revs) {
  //     if (leftD.rotation(rotationUnits::rev) < revs) {
  //       leftD.spin(vex::directionType::fwd, speed, vex::velocityUnits::pct);
  //     }
  //     if (rightD.rotation(rotationUnits::rev) < revs) {
  //       rightD.spin(vex::directionType::fwd, speed, vex::velocityUnits::pct);
  //     }
  //   }
  // }

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


 

void matchside2test(){
  Intake(66.7);
  wait(2,sec);
  turn(-71.5);

 leftD.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);
  wait(0.8, sec);
  leftD.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);
  wait(0.5, sec);
  bc_flip();
  wait(0.2, sec);
  leftD.spin(vex::directionType::fwd, 80, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::fwd, 80, vex::velocityUnits::pct);
  wait(2.0,sec);
  stopDrive();



}
void elimpos1(){
// Intake(66.7);
// straight(120);
// straight(40,30);
// wait(0.25,sec);
// fc_flip(); //could be bc could be fc test to find out
// wait(0.25,sec);
// turn(90); //adjust once first distence is set
// Lift(-1);
// straight(-50);
// straight(-40,30);
// wait(0.5,sec);
// bc_flip(); //play arround to test
// wait(0.5,sec);
// Lift(0);
// turn(-90);
// straight(200);

  leftD.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);
  wait(0.8, sec);
  leftD.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);
  wait(0.5, sec);
  bc_flip();
  wait(0.2, sec);
  leftD.spin(vex::directionType::fwd, 80, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::fwd, 80, vex::velocityUnits::pct);
  wait(2.0,sec);
  stopDrive();

//70 is arround 1 cube

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
void skillsv3(){
Intake(66.7);
Lift(-1);
straight(-20,10);
bc_flip();
straight(5,20);
turn(20,20);
straight(5,20);
turn(20,20);
straight(5,20);
turn(22,20);
leftD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
rightD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
wait(2.625,sec);
fc_flip(); //grabs first yellow
wait(0.75,sec);
stopDrive();
Lift(50);
wait(2,sec);
Lift(20);
turn(36,20);
straight(95,30);
turn(-8);
Lift(0);
wait(1,sec);
fc_flip(); //release first yellow
turn(-2,10); //added since changed 201 to 8 instead of 10
leftD.spin(vex::directionType::rev, 30, vex::velocityUnits::pct);
rightD.spin(vex::directionType::rev, 30, vex::velocityUnits::pct);
Lift(20);
wait(0.5,sec);
Lift(0);
stopDrive();
straight(-30,30);
Lift(-50);
wait(2.01,sec);
Lift(-1);
bc_flip();
turn(189);
leftD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
rightD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
Lift(0);
wait(0.8,sec);
fc_flip(); //grabs first blue
Lift(0);
wait(0.25,sec);
stopDrive();
turn(-10);
straight(-79,20);//origianlly was 80
turn(-70); //changed and adjusted
leftD.spin(vex::directionType::rev, 30, vex::velocityUnits::pct);
rightD.spin(vex::directionType::rev, 30, vex::velocityUnits::pct);
wait(0.75,sec);
bc_flip(); //grabs red
wait(0.25,sec);
stopDrive();
straight(180);
Lift(100);
wait(1.2,sec);
Lift(1);
turn(-70);
straight(5,20);
Lift(0);
wait(0.25,sec);
fc_flip();
wait(0.25,sec); //released blue
//done
Intake(66.7);
turn(90);
straight(80,40);
turn(95);
//turn to yellow
Lift(-90);
wait(1.3,sec);
Lift(-1);
leftD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
rightD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
wait(1,sec);
Lift(0);
wait(0.25,sec);
fc_flip();
wait(2,sec);
turn(90);





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

  // Default starting
  fClawSet = false;
  bClawSet = false;
  setClaws();
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
  //autonskills80pnts();
  //matchauton_pos1();
  //matchauton_pos2(); not it
  //elimpos1();
  //skills400();

  //v2 is it
  straight(100);
  // skillsv2();
  //skillsv4();
  //skillsv3();
  //stepbysteptest();
  
  //matchside2test();
  //elimpos2() elimpos 2 is not it, elim pos 1 is
  //auton_sequence();
  // turn(90);
  // turn(-90);
  // turn(180);
  // turn(-180);
  // turn(360);

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
  
  bool slowerDrive = false;
  int yCoolDown = 0;
  float driveMultiple = 1.0;

  // Must actuation button disabled for this amount of time after pressing
  // Makes it so that the piston doesn't shoot back and forth in the time of pressing the button
  int coolDownThreshold = 200;

  // Main loop
  while (1) {

    // Increment cooldowns
    fCoolDown += cycleTime;
		bCoolDown += cycleTime;
    yCoolDown += cycleTime;
    
    // Set tank drive
    if (slowerDrive) {
      Intake(500);
      driveMultiple = 0.5;
    } else {
      Intake(-500);
      driveMultiple = 1.0;
    }
    leftD.spin(vex::directionType::fwd, Controller1.Axis3.value() * driveMultiple, vex::velocityUnits::pct);
    rightD.spin(vex::directionType::fwd, Controller1.Axis2.value() * driveMultiple, vex::velocityUnits::pct);

    // Intake

    // Special intake configuration: disable intake or flip direction
    // Assumption is that we want the intake to run by default for convenience picking up rings
    if (Controller1.ButtonX.pressing()) {
			intakeOn = !intakeOn;
		}
		if (intakeOn) {
			Intake(66.7); //changed from 100 to 66.7
		} else {
			Intake(0);
		}
		if (Controller1.ButtonB.pressing()) {
			Intake(-100);
		}
    if (Controller1.ButtonY.pressing() && yCoolDown > coolDownThreshold) {
      slowerDrive = true;
      yCoolDown = 0;
    }
    else if (Controller1.ButtonA.pressing() && yCoolDown > coolDownThreshold) {
      slowerDrive = false;
      yCoolDown = 0;
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