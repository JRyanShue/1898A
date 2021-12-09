#include "main.h"
#include "display/lvgl.h"
#include "pros/apix.h"

using namespace okapi;

#define LF -7
#define LM 2
#define LB -3
#define RF 4
#define RM -5
#define RB 6
#define INTAKE -9
#define LIFT 10
#define FCLAW 'A'
#define BCLAW 'B'

pros::Motor lF(LF);
pros::Motor rF(RF);
MotorGroup L({LF, LM, LB});
MotorGroup R({RF, RM, RB});
Motor Intake(INTAKE);
Motor Lift(LIFT);
bool fClawSet = true;
bool bClawSet = true;
pros::ADIDigitalOut fClaw (FCLAW);
pros::ADIDigitalOut bClaw (BCLAW);
pros::Rotation rL(LF);
pros::Rotation rR(RF);

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {

	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);

	// Activate both pistons to be within size limit
	fClaw.set_value(fClawSet);
	bClaw.set_value(bClawSet);

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void straightMove(int voltage) {

	L.moveVoltage(voltage);
	R.moveVoltage(voltage);

}
void stopDrive(){

	straightMove(0);

}
void wait(int ms){

	pros::delay(ms);

}
void intake(int voltage){

	Intake.moveVoltage(voltage);

}
void lift(int voltage){

	Lift.moveVoltage(voltage);

}
void holdDrive() {

	L.setBrakeMode(AbstractMotor::brakeMode(MOTOR_BRAKE_HOLD));
	R.setBrakeMode(AbstractMotor::brakeMode(MOTOR_BRAKE_HOLD));

}
void moveDrive(int distance, float speed = 0.5, int timeOut = 2000) {

	int defaultVoltage = speed * 12000.0;
	int startTime = pros::millis();

	int lOrigin = L.getPosition();
	int rOrigin = R.getPosition();

	int lPos = L.getPosition() - lOrigin;
	int rPos = R.getPosition() - rOrigin;

	if (distance >= 0) {

		while (lPos < distance && rPos < distance && pros::millis() - startTime < timeOut) {
			lPos = L.getPosition() - lOrigin;
			rPos = R.getPosition() - rOrigin;
			if (lPos < distance) {
				L.moveVoltage(defaultVoltage);
			} else {
				L.moveVoltage(0);
			} if (rPos < distance) {
				R.moveVoltage(defaultVoltage);
			} else {
				R.moveVoltage(0);
			}
		}

	} else {

		while (lPos > distance && rPos > distance && pros::millis() - startTime < timeOut) {
			lPos = L.getPosition() - lOrigin;
			rPos = R.getPosition() - rOrigin;
			if (lPos > distance) {
				L.moveVoltage(-defaultVoltage);
			} else {
				L.moveVoltage(0);
			} if (rPos > distance) {
				R.moveVoltage(-defaultVoltage);
			} else {
				R.moveVoltage(0);
			}
		}
	}

	stopDrive();

}
void turnDrive(float angle, float speed = 0.5, int timeOut = 2000) {

	int lOrigin = L.getPosition();
	int rOrigin = R.getPosition();

	float c = 5.0;
	int distance = angle * c;
	int defaultVoltage = speed * 12000.0;
	int startTime = pros::millis();
	if (angle >= 0) {

		int lPos = L.getPosition() - lOrigin;
		int rPos = R.getPosition() - rOrigin;
		while (lPos < distance && rPos > -distance && pros::millis() - startTime < timeOut) {
			lPos = L.getPosition() - lOrigin;
			rPos = R.getPosition() - rOrigin;
			if (lPos < distance) {
				L.moveVoltage(defaultVoltage);
			} else {
				L.moveVoltage(0);
			} if (rPos > -distance) {
				R.moveVoltage(-defaultVoltage);
			} else {
				R.moveVoltage(0);
			}
		}

	} else {

		int lPos = L.getPosition() - lOrigin;
		int rPos = R.getPosition() - rOrigin;
		while (lPos > distance && rPos < -distance && pros::millis() - startTime < timeOut) {
			lPos = L.getPosition() - lOrigin;
			rPos = R.getPosition() - rOrigin;
			if (lPos > distance) {
				L.moveVoltage(defaultVoltage);
			} else {
				L.moveVoltage(0);
			} if (rPos < -distance) {
				R.moveVoltage(-defaultVoltage);
			} else {
				R.moveVoltage(0);
			}
		}

	}

	stopDrive();

}
void bFlip() {

	bClawSet = !bClawSet;
	bClaw.set_value(bClawSet);

}
void fFlip() {

	fClawSet = !fClawSet;
	fClaw.set_value(fClawSet);

}
void rMatchAuton() {

	// Drive backwards, grab goal, dump ring, drive forwards
	bFlip(); fFlip();
	straightMove(-3000); wait(2000);
	bFlip(); stopDrive(); intake(3000); wait(800);
	intake(6000); wait(2200);
	straightMove(3000); wait(2000);
	stopDrive();

}
void lMatchAuton() {

	// Score rings, grab neutral goal
	bFlip(); fFlip();
	// intake(3000); wait(800);
	// turnDrive(90, 0.4);
	lift(-1000);
	moveDrive(3500, 0.8); fFlip(); wait(700);
	moveDrive(-3000, 0.6);

}
void autonomous() {

	Lift.setBrakeMode(AbstractMotor::brakeMode(MOTOR_BRAKE_HOLD));
	holdDrive();

	// rMatchAuton();
	lMatchAuton();

}



/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {

	int delayAmount = 20;

	bFlip(); fFlip();
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	bool intakeOn = true;

	// Cooldowns for pistons so that they don't oscillate back and forth
	int fCoolDown = 0;
	int bCoolDown = 0;

	while (true) {

		fCoolDown += delayAmount;
		bCoolDown += delayAmount;

		// Tank drive
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		L.moveVoltage(left * 12000/127);
		R.moveVoltage(right * 12000/127);

		// Intake
		// if (master.get_digital(DIGITAL_L2)) {
		// 	Intake.moveVoltage(12000);
		// } else if (master.get_digital(DIGITAL_L1)) {
		// 	Intake.moveVoltage(-12000);
		// } else {
		// 	Intake.moveVoltage(0);
		// }
		if (master.get_digital(DIGITAL_X)) {
			intakeOn = !intakeOn;
		}
		if (intakeOn) {
			intake(8000);
		} else {
			intake(0);
		}
		if (master.get_digital(DIGITAL_B)) {
			intake(-12000);
		}

		// Lift
		Lift.setBrakeMode(AbstractMotor::brakeMode(MOTOR_BRAKE_HOLD));
		if (master.get_digital(DIGITAL_R1)) {
			Lift.moveVoltage(12000);
		} else if (master.get_digital(DIGITAL_R2)) {
			Lift.moveVoltage(-12000);
		} else {
			Lift.moveVoltage(0);
		}

		// Front Claw
		if (master.get_digital(DIGITAL_L2) && fCoolDown > 200) {
			fClawSet = !fClawSet;
			fCoolDown = 0;
		}

		// Back Claw
		if (master.get_digital(DIGITAL_L1) && bCoolDown > 200) {
			bClawSet = !bClawSet;
			bCoolDown = 0;
		}

		// Set Claws
		fClaw.set_value(fClawSet);
		bClaw.set_value(bClawSet);

		pros::delay(delayAmount);

	}

}
