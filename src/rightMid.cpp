#include "main.h"
using namespace drive;
using namespace intake;
using namespace lift;



void rightAutonMid(){
    setClawUp();
    setMogoCoverDown();
    movePID(145_cm);
    waitUntilSettled();
    setClawDown();
    movePID(-113_cm);
    waitUntilSettled();
    setMogoCoverUp();
    setLiftLow();
    setMogoDown();
    turnTo(-65_deg);
    linearScurveProfile.moveBackwards(70_cm);
    setMogoUp();
    delay(150);
    linearScurveProfile.moveForwardsAsync(50_cm);
    delay(300);
    intakeOn();
    delay(1500);
    setLiftDown();
    intakeOff();
    linearScurveProfile.waitUntilSettled();
    setMogoDown();

}
