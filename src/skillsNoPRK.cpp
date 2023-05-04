#include "main.h"
#include "zprofiles/skills1.h"
#include "zprofiles/skills2.h"
#include "zprofiles/skills3.h"
#include "zprofiles/skills4.h"

    using namespace drive;
    using namespace intake;
    using namespace lift;
void skillsNoPark(){
    setMogoDown();
    delay(200);
    linearScurveProfile.moveBackwards(40_cm);
    setMogoUp();
    setClawUp();
    delay(50);
    linearScurveProfile.movePlannerPath(skills1, false);
    linearScurveProfile.waitUntilSettled();
    setClawDown();
    delay(30);
    setLiftUp();
    intakeOn();
    linearScurveProfile.movePlannerPath(skills2, false);
    linearScurveProfile.waitUntilSettled();
    setLiftMid();
    delay(1500);
    setClawUp();
    delay(50);


    linearScurveProfile.moveBackwardsAsync(5_cm);
    delay(150);
    setLiftHigh();
    linearScurveProfile.waitUntilSettled();
    setLiftLow();
    turnTo(-180_deg);
    linearScurveProfile.moveForwards(110_cm);
    turnTo(-90_deg);
    linearScurveProfile.moveForwardsAsync(65_cm);
    delay(450);
    setLiftDown();
    linearScurveProfile.waitUntilSettled();
    setClawDown();
    delay(100);
    setLiftLow();
    turnToWithMogo(68_deg);
    setLiftHigh();
    linearScurveProfile.moveForwards(108_cm);
    setLiftMid();
    delay(300);
    setClawUp();
    delay(50);
    linearScurveProfile.movePlannerPath(skills4, true);  
    delay(300);
    setLiftHigh();
    delay(500);
    setLiftDown();
    linearScurveProfile.waitUntilSettled();
    
    turnTo(180_deg);
    linearScurveProfile.moveForwards(40_cm);
    setClawDown();
    delay(100);
    setLiftLow();
    turnToWithMogo(60_deg);
    setLiftHigh();
    linearScurveProfile.moveForwards(230_cm);
    setLiftMid();
    delay(400);
    setClawUp();
    delay(50);
    linearScurveProfile.moveBackwardsAsync(65_cm);


    delay(200);
    setLiftHigh();
    linearScurveProfile.waitUntilSettled();
    setLiftDown();
    turnTo(-45_deg);
    linearScurveProfile.moveForwards(75_cm);
    setClawDown();
    delay(150);
    setLiftUp();
    turnToWithMogo(62_deg);
    //setLiftUp();
    linearScurveProfile.moveForwards(128_cm);
    turnToHighMogo(90_deg);
    setLiftMid();
    delay(900);
    setClawUp();
    delay(50);
    linearScurveProfile.moveBackwardsAsync(30_cm);
    delay(400);
    setLiftUp();
    linearScurveProfile.waitUntilSettled();


    setMogoDown();
    delay(400);
    linearScurveProfile.moveForwards(15_cm);
    turnTo(175_deg);
    setLiftDown();
    linearScurveProfile.movePlannerPath(skills3, true);
    linearScurveProfile.waitUntilSettled();
    setMogoUp();
    delay(200);
    turnTo(175_deg);
    setLiftLow();
    linearScurveProfile.moveForwards(260_cm);
    turnTo(58_deg);
    setLiftDown();
    linearScurveProfile.moveForwards(68_cm);
    setClawDown();
    delay(50);
    linearScurveProfile.moveBackwards(68_cm);
    setLiftLow();
    turnTo(-95_deg);
    linearScurveProfile.moveForwardsAsync(100_cm);
    setLiftHigh();
    linearScurveProfile.waitUntilSettled();
    turnTo(-45_deg);
    linearScurveProfile.moveForwards(130_cm);
    turnTo(-65_deg);
    setLiftMid();
    delay(300);
    setClawUp();
    delay(150);
    setLiftHigh();
    linearScurveProfile.moveBackwards(10_cm);
    setMogoDown();


}

/*void skills(){
    setMogoDown();
    delay(200);
    linearScurveProfile.moveBackwards(40_cm);
    setMogoUp();
    setClawUp();
    delay(50);
    linearScurveProfile.movePlannerPath(skills1, false);
    linearScurveProfile.waitUntilSettled();
    setClawDown();
    delay(30);
    setLiftUp();
    intakeOn();
    linearScurveProfile.movePlannerPath(skills2, false);
    linearScurveProfile.waitUntilSettled();
    setLiftMid();
    delay(1000);
    setClawUp();
    delay(50);


    linearScurveProfile.moveBackwardsAsync(5_cm);
    delay(150);
    setLiftHigh();
    linearScurveProfile.waitUntilSettled();
    setLiftLow();
    turnTo(-180_deg);
    linearScurveProfile.moveForwards(110_cm);
    turnTo(-90_deg);
    linearScurveProfile.moveForwardsAsync(65_cm);
    delay(450);
    setLiftDown();
    linearScurveProfile.waitUntilSettled();
    setClawDown();
    delay(100);
    setLiftLow();
    turnTo(68_deg);
    setLiftHigh();
    linearScurveProfile.moveForwards(108_cm);
    setLiftMid();
    delay(300);
    setClawUp();
    delay(50);
    linearScurveProfile.moveBackwardsAsync(108_cm);    
    delay(200);
    setLiftHigh();
    delay(200);
    setLiftLow();
    linearScurveProfile.waitUntilSettled();


    setLiftDown();
    turnTo(-90_deg);
    linearScurveProfile.moveForwards(100_cm);
    turnTo(180_deg);
    linearScurveProfile.moveForwards(40_cm);
    setClawDown();
    delay(150);
    setLiftLow();
    turnTo(64_deg);
    setLiftHigh();
    linearScurveProfile.moveForwards(230_cm);
    setLiftMid();
    delay(400);
    setClawUp();
    delay(50);
    linearScurveProfile.moveBackwardsAsync(55_cm);


    delay(200);
    setLiftHigh();
    linearScurveProfile.waitUntilSettled();
    setLiftDown();
    turnTo(-45_deg);
    linearScurveProfile.moveForwards(75_cm);
    setClawDown();
    delay(150);
    setLiftLow();
    linearScurveProfile.moveForwards(75_cm);
    turnTo(90_deg);
    setLiftUp();
    linearScurveProfile.moveForwards(177_cm);
    setLiftMid();
    delay(900);
    setClawUp();
    delay(50);
    linearScurveProfile.moveBackwardsAsync(30_cm);
    delay(400);
    setLiftUp();
    linearScurveProfile.waitUntilSettled();
/*

    setMogoDown();
    delay(400);
    linearScurveProfile.moveForwards(20_cm);
    turnTo(180_deg);
    setLiftDown();
    linearScurveProfile.movePlannerPath(skills3, true);
    linearScurveProfile.waitUntilSettled();
    setMogoUp();
    delay(200);
    turnTo(180_deg);
    setLiftLow();
    linearScurveProfile.moveForwards(260_cm);
    turnTo(58_deg);
    setLiftDown();
    linearScurveProfile.moveForwards(68_cm);
    setClawDown();
    delay(50);
    linearScurveProfile.moveBackwards(68_cm);
    setLiftLow();
    turnTo(-90_deg);
    linearScurveProfile.moveForwardsAsync(240_cm);
    delay(1200);
    setLiftHigh();
    linearScurveProfile.waitUntilSettled();
    turnTo(0_deg);
    linearScurveProfile.moveForwards(15_cm);
    setLiftDown();
    linearScurveProfile.moveForwards(100_cm);
    

}

/*void skills(){ //8059
    using namespace drive;
    using namespace intake;
    using namespace lift;

    setMogoDown();
    linearScurveProfile.moveBackwards(40_cm);
    setMogoUp();
    setClawUp();
    delay(50);
    linearScurveProfile.movePlannerPath(skills1, false);
    linearScurveProfile.waitUntilSettled();
    setClawDown();
    delay(30);
    setLiftUp();
    intakeOn();
    linearScurveProfile.movePlannerPath(skills2, false);
    linearScurveProfile.waitUntilSettled();
    linearScurveProfile.moveBackwards(20_cm);
    turnTo(90_deg);
    linearScurveProfile.moveForwards(40_cm);

    setLiftMid();
    delay(1000);
    setClawUp();
    delay(300);
    setLiftHigh();
    linearScurveProfile.moveBackwards(30_cm);
    setLiftDown();
    turnTo(-90_deg);
    setMogoDown();
    delay(200);
    linearScurveProfile.moveForwards(45_cm);
    intakeOff();
    setClawDown();
    delay(150);
    setLiftLow();
    intakeReverse();
    linearScurveProfile.moveForwards(95_cm);
    turnTo(0_deg);
    setLiftDown();
    intakeOff();
    delay(200);
    setClawUp();
    delay(150);
    linearScurveProfile.movePlannerPath(skills3, true);
    setMogoUp();
    delay(200);
    linearScurveProfile.moveForwards(45_cm);
}*/