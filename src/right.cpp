#include "main.h"
using namespace drive;
using namespace intake;
using namespace lift;



void rightAuton(){
    setClawUp();
    setMogoCoverDown();
    movePID(118_cm);
    delay(865);
    setClawDown();
    waitUntilSettled();
    movePID(-130_cm);
    waitUntilSettled();
    setMogoCoverUp();
    setLiftLow();
    setMogoDown();
    linearScurveProfile.moveForwards(40_cm);
    turnTo(-90_deg);
    linearScurveProfile.moveBackwards(45_cm);
    setMogoUp();
    delay(150);
    linearScurveProfile.moveForwards(10_cm);
    intakeOn();
    turnTo(0_deg);
    linearScurveProfile.moveForwards(80_cm);
    linearScurveProfile.moveBackwards(100_cm);
    setMogoDown();
    

}
