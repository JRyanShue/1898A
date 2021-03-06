
// Must include this to use vexcode API
#include "vex.h"
#include <cmath>

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

// void calculate_voltage(float *error, float prev_error, float *integral, float derivative, float kP, float kI, float kD, float dT, ) {

//   // Calculate the output voltage of the PID controller
//   integral += error * dT;  // accumulated error over time to counteract lowering in error
//   if (error <= 0.0 || error > 3.0) {  // Disable the integral from overaccumulation or factoring in after hitting the target
//     integral = 0.0;
//   }
//   derivative = (error - prev_error)/dT;  // difference in error from last cycle to this -- this value is negative!

//   // Three variables that factor into voltage setting
//   p = kP * error;
//   i = kI * integral;
//   float d = kD * derivative;
//   float voltage_percent = p + i + d;

//   return voltage_percent, p, i, d;

// }

// Straight drive movement
void straight(float dist, float _time=1000.0) {

  // PID constants
  float kP = 50.0; //until goes past then little back
  float kI = 0.03; //add until no steady state error so you hit exact every time
  float kD = 43.0; //increase until how far it is when it stops decreases 

  float l_error = 0.0;
  float r_error = 0.0;
  float l_integral = 0.0;
  float r_integral = 0.0;
  float l_derivative = 0.0;
  float r_derivative = 0.0;

  float p;
  float i;
  float d;

  float dT = 5.0;  // ms between update cycles

  // 50 units is one revolution
  float revs = (static_cast<float>(dist)) / 50.0;

  // Initialize previous error
  float l_prev_error = revs;
  float r_prev_error = revs;

  bool reverse = dist < 0;

  float current_pos;
  float l_pos;
  float r_pos;

  resetDrive();

  float time_elapsed = 0;

  float spinup_val = 0.1;

  // Set left side voltage
  // For now, assume forward
  while (true) {

    // current_pos = leftD.rotation(rotationUnits::rev);  // current position
    l_pos = leftD.rotation(rotationUnits::rev);
    r_pos = rightD.rotation(rotationUnits::rev);

    float l_error = revs - l_pos;  // distance to target
    float r_error = revs - r_pos;  // distance to target
    if (reverse) {  // IF REVERSED
      l_error = -l_error;
      r_error = -r_error;
    }
    if (l_error < 0.1 && r_error < 0.1 && time_elapsed > _time) {  // End move if error is small enough
      break;
    }

    l_integral += l_error * dT;  // accumulated error over time to counteract lowering in error
    r_integral += r_error * dT;
    // if (l_error <= 0.0 || l_error > 3.0) {  // Disable the integral from overaccumulation or factoring in after hitting the target
    //   l_integral = 0.0;
    // }
    // if (r_error <= 0.0 || r_error > 3.0) {
    //   r_integral = 0.0;
    // }
    if (l_error > 3.0) {  // Disable the integral from overaccumulation or factoring in after hitting the target
      l_integral = 0.0;
    }
    if (r_error > 3.0) {
      r_integral = 0.0;
    }

    l_derivative = (l_error - l_prev_error)/dT;  // difference in error from last cycle to this -- this value is negative!
    r_derivative = (r_error - r_prev_error)/dT;

    // Three variables that factor into voltage setting
    p = kP * l_error;
    i = kI * l_integral;
    d = kD * l_derivative;

    // Set drive
    if (!reverse) {
      leftD.spin(vex::directionType::fwd, spinup_val * (p + i + d), vex::velocityUnits::pct);
    } else {
      leftD.spin(vex::directionType::rev, spinup_val * (p + i + d), vex::velocityUnits::pct);
    }

    p = kP * r_error;
    i = kI * r_integral;
    d = kD * r_derivative;

    if (!reverse) {
      rightD.spin(vex::directionType::fwd, spinup_val * (p + i + d), vex::velocityUnits::pct);
    } else {
      rightD.spin(vex::directionType::rev, spinup_val * (p + i + d), vex::velocityUnits::pct);
    }

    l_prev_error = l_error;
    r_prev_error = r_error;

    // Wait a constant delay
    wait(dT, msec);
    time_elapsed += dT;

    // Spinup
    if (spinup_val < 1.0) {
      spinup_val += dT * 0.001;
    }

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
void turn(float angle, float _time=1000.0) {

  // PID constants
  float kP = 60.0; //until goes past then little back
  float kI = 0.05; //add until no steady state error so you hit exact every time
  float kD = 90.0; //increase until how far it is when it stops decreases 

  float l_error = 0.0;
  float r_error = 0.0;
  float l_integral = 0.0;
  float r_integral = 0.0;
  float l_derivative = 0.0;
  float r_derivative = 0.0;

  float p;
  float i;
  float d;

  float dT = 5.0;  // ms between update cycles

  // 50 units is one revolution
  float tuneVal = 145.0;
  float revs = (static_cast<float>(angle)) / tuneVal;

  // Initialize previous error
  float l_prev_error = revs;
  float r_prev_error = revs;

  bool reverse = angle < 0;

  float l_pos;
  float r_pos;

  resetDrive();

  float time_elapsed = 0;

  // Set left side voltage
  // For now, assume forward
  while (true) {    

    // current_pos = leftD.rotation(rotationUnits::rev);  // current position
    l_pos = leftD.rotation(rotationUnits::rev);
    r_pos = rightD.rotation(rotationUnits::rev); 

    float l_error = revs - l_pos;  // distance to target
    float r_error = - revs - r_pos;  // distance to target
    if (!reverse) {  // DIRECTION-DEPENDENT
      r_error = -r_error;
    } else {
      l_error = -l_error;
    }
    if (l_error < 0.05 && r_error < 0.05 && time_elapsed > _time) {  // End move if error is small enough
      break;
    }

    l_integral += l_error * dT;  // accumulated error over time to counteract lowering in error
    r_integral += r_error * dT;
    // if (l_error <= 0.0 || l_error > 3.0) {  // Disable the integral from overaccumulation or factoring in after hitting the target
    //   l_integral = 0.0;
    // }
    // if (r_error <= 0.0 || r_error > 3.0) {
    //   r_integral = 0.0;
    // }
    if (l_error > 3.0) {  // Disable the integral from overaccumulation or factoring in after hitting the target
      l_integral = 0.0;
    }
    if (r_error > 3.0) {
      r_integral = 0.0;
    }

    l_derivative = (l_error - l_prev_error)/dT;  // difference in error from last cycle to this -- this value is negative!
    r_derivative = (r_error - r_prev_error)/dT;

    // Three variables that factor into voltage setting
    p = kP * l_error;
    i = kI * l_integral;
    d = kD * l_derivative;

    // Set drive
    if (!reverse) {
      leftD.spin(vex::directionType::fwd, p + i + d, vex::velocityUnits::pct);
    } else {
      leftD.spin(vex::directionType::rev, p + i + d, vex::velocityUnits::pct);
    }
    
    p = kP * r_error;
    i = kI * r_integral;
    d = kD * r_derivative;

    if (!reverse) {
      rightD.spin(vex::directionType::rev, p + i + d, vex::velocityUnits::pct);
    } else {
      rightD.spin(vex::directionType::fwd, p + i + d, vex::velocityUnits::pct);
    }

    l_prev_error = l_error;
    r_prev_error = r_error;

    // Wait a constant delay
    wait(dT, msec);
    time_elapsed += dT;

  }

  // // TUNE TURN AMOUNT: HIGHER VALUE IS SHORTER TURN
  // float tuneVal = 145.0;

  // float revs = (static_cast<float>(angle)) / tuneVal;

  // bool reverse = angle < 0;
  // // int error;

  // resetDrive();

  // if (reverse) {
  //   while (leftD.rotation(rotationUnits::rev) > revs || rightD.rotation(rotationUnits::rev) < -revs) {
  //     if (leftD.rotation(rotationUnits::rev) > revs) {
  //       leftD.spin(vex::directionType::rev, speed, vex::velocityUnits::pct);
  //     }
  //     if (rightD.rotation(rotationUnits::rev) < -revs) {
  //       rightD.spin(vex::directionType::fwd, speed, vex::velocityUnits::pct);
  //     }
  //   }
  // } else {
  //   while (leftD.rotation(rotationUnits::rev) < revs || rightD.rotation(rotationUnits::rev) > -revs) {
  //     if (leftD.rotation(rotationUnits::rev) < revs) {
  //       leftD.spin(vex::directionType::fwd, speed, vex::velocityUnits::pct);
  //     }
  //     if (rightD.rotation(rotationUnits::rev) > -revs) {
  //       rightD.spin(vex::directionType::rev, speed, vex::velocityUnits::pct);
  //     }
  //   }
  // }

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

void auton (void){
  //grab the mogle 1 on ramp
  straight(-20,500);
  bc_flip();
  straight(20,500);
  turn(15,250);
  straight(15,250);
  turn(30,250);
  straight(20,250);
  turn(71,250);
  //goes to yellow mogle( mogal 2)
  straight(100,500);
  Lift(-1);
  straight(50,250);
  Lift(0);
  straight(15,250);
  fc_flip();
  //goes to drop off mogal goal
  wait(0.25,sec);
  stopDrive();
  turn(17,250);
  Lift(100);
  wait(1.6,sec);
  Lift(1);
  straight(110,1000);
  wait(0.25,sec);
  straight(55,1000);
  wait(0.5,sec);
  Lift(0);
  wait(0.5,sec);
  fc_flip();
  straight(-75,250);
  //releases and raises blue
  bc_flip();
  Lift(-100);
  wait(1,sec);
  Lift(-1);
  straight(30,250);
  turn(150);
  straight(45,250);
  fc_flip();
  wait(0.5,sec);
  straight(-75,250);
  turn(-110);
  //turn(195);
  // Lift(100);
  // wait(1.5,sec);
  // Lift(1);
  // straight(100,500);
  // turn(-45,250);
  // straight(40,250);
  // Lift(0);
  // wait(0.5,sec);
  // fc_flip();
  // wait(0.5,sec);
  //straight(-50);



 
  
  

}



void elim1 (void){
  leftD.spin(vex::directionType::rev, -90, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, -90, vex::velocityUnits::pct);
  wait(1,sec);
  fc_flip();
  wait(0.1,sec);
  stopDrive();
  wait(0.25,sec);
  Lift(100);
  leftD.spin(vex::directionType::rev, 90, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, 90, vex::velocityUnits::pct);
  wait(1,sec);
  wait(0.25,sec);
  Lift(0);
  stopDrive();



}
void qual1 (void){
  Lift(20);
  straight(-60,250);
  straight(-35,250);
  bc_flip();
  Lift(0);
  wait(0.5,sec);
  Intake(66.7);
  straight(50,250);
  bc_flip();
  Intake(0);
  Lift(-20);
  wait(0.5,sec);
  straight(25,250);
  turn(113);
  Lift(0);
  straight(105,250);
  leftD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
  wait(1,sec);
  fc_flip();
  wait(0.3,sec);
  straight(-120,250);
  
}
void qual2 (void){
  //grab the mogle 1 on ramp
  straight(-20,500);
  bc_flip();
  Intake(30.7);
  straight(20,500);
  turn(15,250);
  straight(15,250);
  turn(30,250);
  straight(20,250);
  turn(73,250);
  Intake(0);
  //goes to yellow mogle( mogal 2)
  Lift(-5);
  straight(100,500);
  Lift(0);
  leftD.spin(vex::directionType::rev, -20, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, -20, vex::velocityUnits::pct);
  Lift(-1);
  wait(1.0,sec);
  Lift(0);
  fc_flip();
  //goes to drop off mogal goal
  wait(0.5,sec);
  stopDrive();

  straight(-110,250);
  bc_flip();
  straight(20,250);
  

}
void qual3thereternoftheclawbot (void){
  straight(-20,500);
  bc_flip();
  Intake(30.7);
  wait(0.6,sec);
  Intake(0);
  bc_flip();
  straight(50,250);
  turn(90,250);
  straight(75,250);
  turn(-93,250);
  straight(-330,250);
  bc_flip();
  wait(0.2,sec);
  stopDrive();
  wait(0.4,sec);
  Intake(15);
  wait(1,sec);
  straight(100,250);
  bc_flip();
  straight(50,250);


}
void auton2 (void){
  //grab the mogle 1 on ramp
  straight(-20,500);
  bc_flip();
  straight(20,500);
  turn(15,250);
  straight(15,250);
  turn(30,250);
  straight(20,250);
  turn(71,250);
  //goes to yellow mogle( mogal 2)
  straight(110,500); //was 130
  Lift(-1);
  straight(20,250);
  Lift(0);
  straight(35,250); //was 35
  fc_flip();
  turn(22,250);
 //lifts up mogle
  Lift(100);
  wait(1.5,sec);
  Lift(1);
  straight(120,250);
  straight(30,250);
  Lift(0);
  wait(0.5,sec);
  fc_flip();
  wait(0.5,sec);
  //just released yellow
  straight(-50,250);
  //changes starting mogle arm
  bc_flip();
  Lift(-100);
  wait(1,sec);
  Lift(-1);
  straight(30,250);
  turn(150);
  straight(45,250);
  fc_flip();
  wait(0.5,sec);
  straight(-60,250);
  turn(-111);
  //turn to other side mogle
  straight(-85,250);
  straight(-26,250);
  bc_flip();
  //just grabbed red
  wait(0.25,sec);
  straight(190,500);
  Lift(100);
  wait(1.4,sec);
  Lift(1);
  turn(-110,250);
  straight(30,250);
  Lift(0);
  wait(0.25,sec);
  fc_flip();
  wait(0.25,sec);
  straight(-40,250);
  
  Lift(35);
  //goes to get yellowbellowchochellomywello
  turn(93,250);
  Lift(0);
  //wall time
  Intake(66.7);
  leftD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
  rightD.spin(vex::directionType::rev, -40, vex::velocityUnits::pct);
  wait(3.5,sec);
  stopDrive();
  Intake(0);
  Lift(-35);
  straight(-90);
  turn(93,250);
  Lift(0);
  straight(100,250);
  straight(30,250);
  fc_flip();

//grabs yellow
  turn(47,250);
  Lift(100);
  wait(1.5,sec);
  Lift(1);
  straight(105,250);
  turn(-50,250);
  
  Lift(0);
  straight(20);
  wait(0.25,sec);
  fc_flip();
  wait(0.25,sec);
  //puts down yellow, bellow flips
  straight(-50,250);
  bc_flip();
  Lift(-100);
  wait(1,sec);
  Lift(-1);
  straight(30,250);
  turn(150);
  straight(45,250);
  fc_flip();
  wait(0.5,sec);
  straight(-60,250);

  //should grab yellow
  

  

}
void auton3 (void){
  //grab the mogle 1 on ramp
  straight(-20,500);
  bc_flip();
  straight(20,500);
  turn(15,250);
  straight(15,250);
  turn(30,250);
  straight(20,250);
  turn(71,250);
  //goes to yellow mogle( mogal 2)
  straight(110,500); //was 130
  Lift(-1);
  straight(20,250);
  Lift(0);
  straight(35,250); //was 35
  fc_flip();
  turn(22,250);
 //lifts up mogle
  Lift(100);
  wait(1.5,sec);
  Lift(1);
  straight(120,250);
  straight(30,250);
  Lift(0);
  wait(0.5,sec);
  fc_flip();
  wait(0.5,sec);
  //just released yellow
  straight(-50,250);
  //changes starting mogle arm
  bc_flip();
  Lift(-100);
  wait(1,sec);
  Lift(-1);
  straight(30,250);
  turn(150);
  straight(45,250);
  fc_flip();
  wait(0.5,sec);
  straight(-60,250);
  turn(-111);
  //turn to other side mogle
  straight(-85,250);
  straight(-26,250);
  bc_flip();
  //just grabbed red
  wait(0.25,sec);
  straight(190,500);
  Lift(100);
  wait(1.4,sec);
  Lift(1);
  turn(-110,250);
  straight(30,250);
  Lift(0);
  wait(0.25,sec);
  fc_flip();
  wait(0.25,sec);
  straight(-40,250);
  turn(160);
  Intake(66.7);
  //goes past yellow
  straight(75,250);
  straight(75,250);
  Lift(-100);
  Intake(0);
  turn(-80,250);
  straight(55,250);
  Lift(-1);
  turn(-100,250);
  straight(40,250);
  straight(20,250);
  fc_flip();
  //grabs it
  Lift(0);
  wait(0.25,sec);
  straight(-100,250);
  turn(-90,250);
  Lift(100);
  wait(1.5,sec);
  Lift(1);
  straight(75,250);
  turn(-90,250);
  straight(10,250);
  Lift(0);
  wait(0.25,sec);
  //put down mogal
  fc_flip();
  wait(0.25,sec);
  straight(-50,250);
   bc_flip();
  Lift(-100);
  wait(1,sec);
  Lift(-1);
  straight(30,250);
  turn(150);
  straight(45,250);
  fc_flip();
  wait(0.5,sec);
  straight(-60,250);
  turn(-111);
  Lift(100);
  wait(1.5,sec);
  Lift(1);
  turn(-80);
  Lift(0);
  wait(0.25,sec);
  fc_flip();
  wait(0.25,sec);
  straight(-40);
}
void test (void){
  turn(160);

  
  
  

  

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

//auton is the skills run
//elim1 grabs a yellow, place as close as possible
//qual 1 is line side
//qual 2 is ramp side and gets yellow
//qual 3 does both,
//qual 2 and 3 are auton start spots

auton3();
//auton2();
 //auton();
  //elim1();
   //qual1();
  //qual2();
  //qual3thereternoftheclawbot();
  //test();



  //v2 is it
  // // straight(100, 2000.0);
  // turn(90, 2000.0);
  // turn(-90, 2000.0);
  // straight(-100, 2000.0);
  // // skillsv2();
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