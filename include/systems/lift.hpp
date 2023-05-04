#pragma once
#include "main.h"

namespace lift{

//up mid down
struct Positions {
    const double up;
    const double high;
    const double mid;
    const double low;
    const double down;
};

extern Motor liftMotor;
extern pros::Rotation liftRot;

extern void setLiftUp();
extern void setLiftDown();
extern void setLiftMid();
extern void setLiftLow();
extern void setLiftHigh();
extern void setLiftMidHigh();

extern void liftDriverControl();
extern void liftClawDriverControl();
extern void printLiftEncoder();
extern void setClawUp();
extern void setClawDown();

extern void mogoHolderDriverControl();
extern void setMogoUp();
extern void setMogoDown();
extern void setMogoCoverDown();
extern void setMogoCoverUp();

}
