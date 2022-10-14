#include "main.h"

void opcontrol() {
	intake::toggle = false;
	intake::intakeOff();
	while (true) {
        auto fwd = drive::controller.getAnalog(ControllerAnalog::leftY);
        auto turn = drive::controller.getAnalog(ControllerAnalog::rightX);

		drive::driveChassisPtr->arcade(fwd, turn);

		lift::liftDriverControl();
		lift::printLiftEncoder();
		lift::liftClawDriverControl();
		lift::mogoHolderDriverControl();
		intake::intakeDriverControl();
		//drive::printOdomEncoders();
		//drive::printOdomPos();
		delay(10);

	}
}
