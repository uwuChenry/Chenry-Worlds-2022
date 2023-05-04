#include "main.h"
#include "zprofiles/skills4.h"

void autonomous() {
    //using drive::profileController;
    using drive::turnTo;
    using drive::driveChassis;
    //using drive::linearProflie;
    using drive::linearScurveProfile;
    using namespace drive;
    using namespace intake;
    using namespace lift;

    //leftAuton();
    //rightAuton();
    //skills();
    //soloAwp();
    //rightAutonMid();
    skillsNoPark();

}