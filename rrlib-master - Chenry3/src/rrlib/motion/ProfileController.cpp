#include "rrlib/motion/ProfileController.hpp"

#include "okapi/api/util/mathUtil.hpp"

namespace RRLib {
using namespace okapi;

RamseteGains::RamseteGains(double zeta, double b)
    : zeta(zeta)
    , b(b) {}

ProfileController::ProfileController(
    std::shared_ptr<ChassisModel> model,
    std::shared_ptr<PoseEstimator> poseEstimator,
    KinematicConstraints constraints,
    ChassisScales scales,
    AbstractMotor::GearsetRatioPair ratioPair,
    RamseteGains gains,
    double headingkP)
    : gains(gains)
    , headingkP(headingkP)
    , model(model)
    , poseEstimator(poseEstimator)
    , constraints(constraints)
    , scales(scales)
    , ratioPair(ratioPair) {
    task = new pros::Task(trampoline, this);
}

QAngularSpeed ProfileController::calculateDesiredOmega() {
    auto profile = generatedProfile.value();
    auto segment = profile.path[currentStep];

    auto pointA = (currentStep < profile.length - 1) ? profile.path[currentStep + 1] : segment;
    auto pointB = (currentStep < profile.length - 1) ? segment : profile.path[currentStep - 1];

    auto headingDifference = math::constrainAngle(radian * (pointA.heading - pointB.heading));

    return radps * (headingDifference.convert(radian) / segment.dt);
}

void ProfileController::setCorrectionMode(CorrectionMode mode) {
    correctionMode = mode;
}

ProfileController::CorrectionMode ProfileController::getCorrectionMode() {
    return correctionMode;
}

ProfileController::LinearAndAngular ProfileController::calculateVelocitiesRamsete() {
    double width = scales.wheelTrack.convert(meter);
    int reversing = generatedProfile.value().needsReversing ? -1 : 1;
    auto segment = generatedProfile.value().path[currentStep];

    auto poseError = getError();
    auto pose = poseEstimator->getPose();
    auto positionError = poseError.position;

    auto desiredVelocity = segment.velocity * mps * reversing;
    auto desiredOmega = calculateDesiredOmega();

    auto theta = -pose.heading.convert(radian);

    double e1 = std::cos(theta) * positionError.getX().convert(meter) + std::sin(theta) * positionError.getY().convert(meter);
    double e2 = -std::sin(theta) * positionError.getX().convert(meter) + std::cos(theta) * positionError.getY().convert(meter);
    double e3 = poseError.heading.convert(radian);

    double k1 = 2 * gains.zeta * std::sqrt((std::pow(desiredOmega.convert(radps), 2) + gains.b * std::pow(desiredVelocity.convert(mps), 2)));
    double k2 = gains.b;
    double k3 = k1;

    double sinc = e3 == 0 ? 1 : std::sin(e3) / e3;

    double u1 = -k1 * e1;
    double u2 = -k2 * desiredVelocity.convert(mps) * sinc * e2 - k3 * e3;

    auto velocity = desiredVelocity * std::cos(e3) - u1 * mps;
    auto omega = desiredOmega - u2 * radps;

    auto turnDifference = mps * (omega.convert(radps) * width / 2.0);

    return std::make_tuple<QAngularSpeed, QAngularSpeed>(
        linearToRotational(velocity - turnDifference), linearToRotational(velocity + turnDifference));
}

ProfileController::LinearAndAngular ProfileController::calculateVelocitiesBasic() {
    double width = scales.wheelTrack.convert(meter);
    int reversing = generatedProfile.value().needsReversing ? -1 : 1;
    auto segment = generatedProfile.value().path[currentStep];

    auto poseError = getError();
    auto pose = poseEstimator->getPose();
    auto positionError = poseError.position;

    auto velocity = segment.velocity * mps * reversing;
    auto omega = calculateDesiredOmega();

    auto turnDifference = mps * (omega.convert(radps) * width / 2.0);

    return std::make_tuple<QAngularSpeed, QAngularSpeed>(
        linearToRotational(velocity - turnDifference), linearToRotational(velocity + turnDifference));
}

ProfileController::LinearAndAngular ProfileController::calculateVelocitiesHeadingCorrect() {
    double width = scales.wheelTrack.convert(meter);
    int reversing = generatedProfile.value().needsReversing ? -1 : 1;
    auto segment = generatedProfile.value().path[currentStep];

    auto poseError = getError();
    auto pose = poseEstimator->getPose();
    auto positionError = poseError.position;

    auto velocity = segment.velocity * mps * reversing;
    auto omega = calculateDesiredOmega() + radps * (headingkP * poseError.heading.convert(radian));

    auto turnDifference = mps * (omega.convert(radps) * width / 2.0);

    return std::make_tuple<QAngularSpeed, QAngularSpeed>(
        linearToRotational(velocity - turnDifference), linearToRotational(velocity + turnDifference));
}

ProfileController::LinearAndAngular ProfileController::calculateVelocities() {
    switch (correctionMode) {
    case CorrectionMode::None:
        return calculateVelocitiesBasic();

    case CorrectionMode::Ramsete:
        return calculateVelocitiesRamsete();

    case CorrectionMode::Heading:
        return calculateVelocitiesHeadingCorrect();
    }

    return std::make_tuple<QAngularSpeed, QAngularSpeed>(0 * radps, 0 * radps);
}

QAngularSpeed ProfileController::linearToRotational(QSpeed linear) const {
    return (linear * (360_deg / (scales.wheelDiameter * 1_pi))) * ratioPair.ratio;
}

void ProfileController::onNewTarget(std::function<void(Pose)> callback) {
    newTargetCallbacks.push_back(callback);
}

void ProfileController::trampoline(void* instance) {
    static_cast<ProfileController*>(instance)->taskLoop();
}

void ProfileController::taskLoop() {
    while (task->notify_take(true, TIMEOUT_MAX)) {
        generateProfile();

        while (deferringMove.load()) {
            pros::delay(1);
        }

        executePath();
        isExecuting = false;
    }
}

bool ProfileController::calculateIfReversed() {
    auto path = currentPath.value();

    auto poseA = path[0];
    auto poseB = path[1];

    auto posDifference = poseB.position - poseA.position;
    posDifference.rotateSelf(-poseA.heading);

    return posDifference.getX().convert(meter) < 0;
}

void ProfileController::generateProfile() {
    std::vector<Waypoint> pfWaypoints;
    auto path = currentPath.value();
    bool needsReversed = calculateIfReversed();

    auto finalPose = path.back();
    lastFinalTarget.emplace(finalPose);
    for (auto& cb : newTargetCallbacks) {
        cb(finalPose);
    }

    for (auto& waypoint : path) {
        double heading = needsReversed ? waypoint.heading.convert(radian) + 1_pi : waypoint.heading.convert(radian);
        pfWaypoints.push_back(
            Waypoint{ waypoint.position.getX().convert(meter), waypoint.position.getY().convert(meter), heading });
    }

    auto fitMode = FIT_HERMITE_CUBIC;

    if (currentFitMode == FitMode::Quintic) {
        fitMode = FIT_HERMITE_QUINTIC;
    }

    TrajectoryCandidate candidate;

    int length = pathfinder_prepare(
        pfWaypoints.data(),
        static_cast<int>(pfWaypoints.size()),
        fitMode,
        samples,
        timestep.load().convert(second),
        constraints.maxVel,
        constraints.maxAccel,
        constraints.maxJerk,
        &candidate);

    if (candidate.length < 0) {
        throw std::runtime_error("Oof I think that's an impossible path...");
    }

    auto trajectory = new Segment[candidate.length];

    if (trajectory == nullptr) {
        // eek.
        if (candidate.laptr) {
            delete candidate.laptr;
        }

        if (candidate.saptr) {
            delete candidate.saptr;
        }

        throw std::runtime_error("Zoinkers! Looks like we couldn't allocate memory for the path. Either it has issues or you're out of memory!");
    }

    pathfinder_generate(&candidate, trajectory);

    // TODO CHECK REVERSING BOIS AND DO LOTS OF MATH AND HEADACHES EARLIER
    // note: it worked for me at worlds there's probably edge cases but i'm retarded lol
    generatedProfile.emplace(GeneratedProfile{ trajectory, candidate.length, needsReversed });
}

void ProfileController::executePath() {
    auto profile = generatedProfile.value();

    for (currentStep = 0; currentStep < profile.length && !isDisabled() && !pros::competition::is_disabled(); currentStep++) {
        updateError();
        auto velocities = calculateVelocities();

        model->left(std::get<0>(velocities).convert(rpm) / toUnderlyingType(ratioPair.internalGearset));
        model->right(std::get<1>(velocities).convert(rpm) / toUnderlyingType(ratioPair.internalGearset));

        pros::delay(timestep.load().convert(millisecond));
    }

    model->stop();

    reset();
}

void ProfileController::setTarget(WaypointSet path) {
    if (!isExecuting.load()) {
        currentPath = path;
        isExecuting = true;
        task->notify();
    }
}

void ProfileController::controllerSet(WaypointSet path) {
    setTarget(path);
}

WaypointSet ProfileController::getTarget() {
    return currentPath.value_or(WaypointSet());
}

Pose ProfileController::getFinalTarget() const {
    return lastFinalTarget.value_or(Pose());
}

void ProfileController::updateError() {
    auto profile = generatedProfile.value();

    if (currentStep > -1 && currentStep <= profile.length) {
        poseEstimator->update();
        auto currPose = poseEstimator->getPose();

        auto pathStep = profile.path[currentStep];

        if (generatedProfile.value().needsReversing) {
            pathStep.heading = math::constrainAngle(radian * (pathStep.heading - 1_pi)).convert(radian);
        }

        Vector2 desiredPosition(pathStep.x * meter, pathStep.y * meter);

        auto posError = desiredPosition - currPose.position;
        auto headingError = math::constrainAngle(radian * (pathStep.heading + currPose.heading.convert(radian)));

        lastMeasuredPose = currPose;

        lastError = Pose(posError, headingError);
    } else {
        lastError.reset();
    }
}

Pose ProfileController::getError() const {
    return lastError.value_or(Pose());
}

void ProfileController::waitUntilSettled() {
    while (!isSettled()) {
        pros::delay(10);
    }
}

bool ProfileController::isSettled() {
    return isDisabled() || !isExecuting.load();
}

void ProfileController::reset() {
    isExecuting = false;
    currentStep = -1;

    if (generatedProfile.has_value() && generatedProfile.value().path != nullptr) {
        delete generatedProfile.value().path;
    }

    currentPath.reset();
    generatedProfile.reset();
    lastMeasuredPose.reset();
}

bool ProfileController::isDisabled() const {
    return disabled.load();
}

void ProfileController::flipDisable() {
    disabled = !disabled.load();
}

void ProfileController::flipDisable(bool isDisabled) {
    disabled = isDisabled;
}

void ProfileController::setTimestep(QTime newTimestep) {
    timestep = newTimestep;
}

QTime ProfileController::getTimestep() {
    return timestep;
}

void ProfileController::setSamples(int sampleCount) {
    samples = sampleCount;
}

int ProfileController::getSamples() {
    return samples;
}

ProfileController::FitMode ProfileController::getFitMode() {
    return currentFitMode;
}

void ProfileController::setFitMode(FitMode mode) {
    currentFitMode = mode;
}

void ProfileController::moveAsync(WaypointSet path) {
    flipDisable(false);
    setTarget(path);
}

void ProfileController::moveAsync(WaypointSet path, QTime timestep) {
    setTimestep(timestep);
    moveAsync(path);
}

void ProfileController::moveToAsync(WaypointSet path) {
    auto wps = path;
    auto currPose = poseEstimator->getPose();
    currPose.heading = currPose.heading * -1;
    wps.insert(wps.begin(), currPose);
    moveAsync(wps);
}

void ProfileController::moveToAsync(Pose pose) {
    WaypointSet points;
    points.push_back(pose);
    moveToAsync(points);
}

void ProfileController::move(WaypointSet path) {
    flipDisable(false);
    setTarget(path);
    waitUntilSettled();
    reset();
}

void ProfileController::move(WaypointSet path, QTime timestep) {
    setTimestep(timestep);
    move(path);
}

void ProfileController::moveTo(WaypointSet path) {
    auto wps = path;
    auto currPose = poseEstimator->getPose();
    currPose.heading = currPose.heading * -1;
    wps.insert(wps.begin(), currPose);
    move(wps);
}

void ProfileController::moveTo(Pose pose) {
    WaypointSet points;
    points.push_back(pose);
    moveTo(points);
}

void ProfileController::deferNext() {
    deferringMove = true;
}

bool ProfileController::isDeferred() {
    return deferringMove.load();
}

void ProfileController::go() {
    deferringMove = false;
}

void ProfileController::goAndWait() {
    go();
    waitUntilSettled();
}

void ProfileController::moveForwardAsync(QLength distance) {
    auto currentTargetPose = getFinalTarget();

    auto increase = Vector2(
        std::cos(-currentTargetPose.heading.convert(radian)) * distance,
        std::sin(currentTargetPose.heading.convert(radian)) * distance);

    auto newTargetPose
        = Pose(Vector2::add(currentTargetPose.position, increase), currentTargetPose.heading);

    moveAsync({ currentTargetPose, newTargetPose });
}

void ProfileController::moveForward(QLength distance) {
    moveForwardAsync(distance);
    waitUntilSettled();
}

void ProfileController::moveBackward(QLength distance) {
    moveForward(distance * -1);
}

void ProfileController::moveBackwardAsync(QLength distance) {
    moveForwardAsync(distance * -1);
}

void ProfileController::tarePosition(){};
WaypointSet ProfileController::getProcessValue() const{};
void ProfileController::setMaxVelocity(std::int32_t imaxVelocity){}; 

}