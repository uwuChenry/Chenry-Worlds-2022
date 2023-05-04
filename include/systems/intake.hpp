#pragma once
namespace intake{

extern void intakeDriverControl();
extern void setIntake(double ispeed);

extern void intakeOn();
extern void intakeReverse();
extern void intakeOff();
extern std::atomic<bool> toggle;
}