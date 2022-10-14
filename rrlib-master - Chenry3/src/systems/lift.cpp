#include "main.h"
#include "systems/lift.hpp"

namespace lift{
Motor liftMotor (2);
//RotationSensor liftRot (20, true);
pros::Rotation liftRot(7);
pros::Optical backMogoOptical(21);
pros::ADIDigitalOut liftClawLeft('B');
pros::ADIDigitalOut liftClawRight('D');
pros::ADIDigitalOut mogoHolder('E');
pros::ADIDigitalOut mogoHolderClaw('A');
pros::ADIDigitalOut mogoCover('C');

int liftCounter = 0;
bool backMogoDown = true;


ControllerButton clawUpButton = drive::controller[ControllerDigital::R1];
ControllerButton clawDownButton = drive::controller[ControllerDigital::R2];

ControllerButton mogoUpButton = drive::controller[ControllerDigital::Y];
ControllerButton mogoDownButton = drive::controller[ControllerDigital::A];

ControllerButton liftUpButton = drive::controller[ControllerDigital::L1];
ControllerButton liftDownButton = drive::controller[ControllerDigital::L2];

ControllerButton liftHighButton = drive::controller[ControllerDigital::X];
ControllerButton liftRestButton = drive::controller[ControllerDigital::B];

pidGains liftGains(1.5, 0.001, 0, 3000, 0);
Positions liftPositions {82000, 55000, 39000, 20000, 0};
long liftTarget = liftPositions.down;
double integral;

Task BackMogoTask{ [] {
    while (Task::notify_take(true, TIMEOUT_MAX)) {

        if (backMogoDown){
            mogoHolder.set_value(false);
            delay(300);
            mogoHolderClaw.set_value(false);
            intake::toggle = false;
        }
        else {
            mogoHolderClaw.set_value(true);
            delay(100);
            mogoHolder.set_value(true);
            if (backMogoOptical.get_proximity() >= 70){
                delay(500);
                intake::toggle = true;
            }
        }
    }
} };



void LiftControl(void*){
    long currentPosition;
    double motorOutput;
    long derivative;
    long lastError;
    liftMotor.setBrakeMode(AbstractMotor::brakeMode::coast);

    while(true){
                pros::lcd::print(0, "%d optical", backMogoOptical.get_proximity());
        currentPosition = liftRot.get_position();
        long error = liftTarget - currentPosition; 
        integral += error;
        integral = std::clamp(integral, (-liftGains.limit/liftGains.kI), (liftGains.limit/liftGains.kI));
        derivative = error - lastError;
        

        if (liftTarget == liftPositions.down && liftRot.get_position() <= liftPositions.down) {
            motorOutput = 0;
            integral = 0;
        }

        if (liftTarget == liftPositions.down){
            integral = 0;
        }

        double finalIntegral = integral * liftGains.kI;
        finalIntegral = std::clamp(finalIntegral, -liftGains.limit, liftGains.limit);
        double output = (error * liftGains.kP) + finalIntegral + (derivative * liftGains.kD);
        lastError = error;
        motorOutput = std::clamp(output, -12000.0, 12000.0);
        liftMotor.moveVoltage(motorOutput);
        pros::lcd::print(4, "integral %f", integral);
        pros::lcd::print(5, "error %f", error);
        pros::lcd::print(6, "output %f", motorOutput);
        delay(10);
    }
}


void setLiftUp(){
    liftTarget = liftPositions.up;
    integral = 0;
}

void setLiftMid(){
    liftTarget = liftPositions.mid;
}

void setLiftMidHigh(){
    liftTarget = liftPositions.high;
}

void setLiftHigh(){
    liftTarget = 65000;
}

void setLiftLow(){
    liftTarget = liftPositions.low;
}

void setLiftDown(){
    liftTarget = liftPositions.down;
    integral = 0;
}

void setClawDown(){
    liftClawLeft.set_value(true);
    liftClawRight.set_value(true);
}

void setClawUp(){
    liftClawLeft.set_value(false);
    liftClawRight.set_value(false);
}

void liftDriverControl(){
    if (liftHighButton.isPressed())
    {
        liftCounter = 3;
    }
    if (liftRestButton.isPressed()){
        liftCounter = 0;
    }
    if (liftUpButton.changedToPressed()){
        liftCounter ++;
    }
    if (liftDownButton.changedToPressed()){
        liftCounter --;
    }
    liftCounter = std::clamp(liftCounter, 0, 5);
    
    switch (liftCounter)
    {
    case 0:
    setLiftDown();
        break;
    case 1:
    setLiftLow();
        break;
    case 2:
    setLiftMid();
        break;
    case 3:
    setLiftMidHigh();
        break;
    case 4:
    setLiftHigh();
        break;
    case 5:
    setLiftUp();
        break;
    default:
    setLiftDown();
        break;
    }
}


void printLiftEncoder(){
    pros::lcd::print(2, "lift encoder %ld", liftRot.get_position());
}

void liftClawDriverControl(){
    if (clawUpButton.isPressed()) setClawUp();
    if (clawDownButton.isPressed()) setClawDown();
}

void mogoHolderDriverControl(){
    if (mogoUpButton.isPressed()) setMogoUp();
    if (mogoDownButton.isPressed()) setMogoDown();
}

void setMogoUp(){
    backMogoDown = false;
    BackMogoTask.notify();
}

void setMogoDown(){
    backMogoDown = true;
    BackMogoTask.notify();
}

void setMogoCoverUp(){
    mogoCover.set_value(true);
}

void setMogoCoverDown(){
    mogoCover.set_value(false);
}




Task LiftControlTask(LiftControl);
}


