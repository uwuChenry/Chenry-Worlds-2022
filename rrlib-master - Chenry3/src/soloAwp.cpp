#include "main.h"
using namespace drive;
using namespace intake;
using namespace lift;

void soloAwp(){
    setLiftLow();
    delay(100);
    setClawUp();
    delay(150);
    linearScurveProfile.moveBackwards(10_cm);
    turnTo(-45_deg);
    linearScurveProfile.moveForwards(70_cm);
    turnTo(180_deg);
    setMogoDown();
    linearScurveProfile.moveBackwards(218_cm);
    setMogoUp();
    delay(150);
    linearScurveProfile.moveForwardsAsync(45_cm);
    delay(300);
    intakeOn();
    linearScurveProfile.waitUntilSettled();
    turnTo(-135_deg);
    setLiftDown();
    linearScurveProfile.moveForwards(92_cm);
    setClawDown();
    delay(100);
    setLiftLow();
    linearScurveProfile.moveBackwards(130_cm);
    setLiftDown();


}
