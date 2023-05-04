#include "main.h"

namespace intake{

std::atomic<bool> toggle = false;
double speed;

Motor intakeMotor(-18);

ControllerButton intakeToggleButton = drive::controller[ControllerDigital::up];
ControllerButton intakeDownButton = drive::controller[ControllerDigital::down];

void intakeDriverControl(){

    if (intakeToggleButton.changedToPressed()){
        toggle = (toggle.load()==1)?false:true;
    }
    if (toggle.load() == true){
        intakeOn();
    }
    if (intakeDownButton.isPressed()){
        intakeReverse();
    }
    if (toggle.load() == false){
        intakeOff();
    }
}

void setIntake(double ispeed){
    speed = ispeed;
}

void intakeOn(){
    setIntake(200);
}

void intakeOff(){
    setIntake(0);
}

void intakeReverse(){
    setIntake(-200);
}

void intakeMoveTaskFunction(void*){
    while (1)
    {
        intakeMotor.moveVelocity(speed);
        delay(10);
    }   
}

Task intakeVelocityTask(intakeMoveTaskFunction);
}
