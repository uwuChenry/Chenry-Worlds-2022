#include "main.h"
using namespace drive;
using namespace intake;
using namespace lift;



void leftAuton(){
    setClawUp();
    setMogoCoverDown();
    movePID(125_cm);
    delay(875);
    setClawDown();
    waitUntilSettled();
    movePID(-60_cm);
    waitUntilSettled();
    setMogoCoverUp();
    setLiftLow();
    turnTo(45_deg);
    linearScurveProfile.moveBackwards(40_cm);
    setMogoDown();
    turnTo(-45_deg);
    linearScurveProfile.moveBackwards(53_cm);
    setMogoUp();
    delay(150);
    linearScurveProfile.moveForwards(15_cm);
    intakeOn();
    delay(500);
    intakeOff();
    setMogoDown();



    

}
