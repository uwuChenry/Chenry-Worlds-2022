#pragma once

#include "rrlib/motion/odometry.hpp"
#include "rrlib/util/structs.hpp"
#include "rrlib/util/math.hpp"

#include "okapi/api/chassis/model/chassisModel.hpp"
#include "okapi/api/control/async/asyncPositionController.hpp"
#include "okapi/api/units/QAngularSpeed.hpp"
#include "okapi/api/units/QSpeed.hpp"
#include "okapi/api/units/QTime.hpp"

extern "C" {
#include "okapi/pathfinder/include/pathfinder.h"
}

#include <atomic>
#include <optional>
#include <tuple>

namespace RRLib {
struct RamseteGains {
    double zeta; // between 0.0 and 1.0, dampens
    double b; // greater than 0.0, increases correction

    RamseteGains(double zeta, double b);
};

struct RamseteOutput {
    okapi::QSpeed velocity;
    okapi::QAngularSpeed omega;
};

using WaypointSet = std::vector<Pose>;

class ProfileController : public okapi::AsyncPositionController<WaypointSet, Pose> {
public:
    enum class FitMode {
        Cubic,
        Quintic
    };

    enum class CorrectionMode {
        None,
        Ramsete,
        Heading
    };

private:
    // Gains for correction types
    RamseteGains gains;
    double headingkP;

    // Objects for chassis
    std::shared_ptr<okapi::ChassisModel> model;
    std::shared_ptr<PoseEstimator> poseEstimator;

    // Scales, ratios, etc
    KinematicConstraints constraints;
    okapi::ChassisScales scales;
    okapi::AbstractMotor::GearsetRatioPair ratioPair;

    // Internal things to kep track of
    pros::Task* task = nullptr;
    std::atomic_bool disabled = false;
    std::atomic_int currentStep = -1;
    std::atomic_bool isExecuting = false;
    std::atomic_bool deferringMove = false;

    // User settings
    std::atomic<okapi::QTime> timestep = 10_ms;
    CorrectionMode correctionMode{ CorrectionMode::None };
    FitMode currentFitMode{ FitMode::Cubic };
    int samples{ PATHFINDER_SAMPLES_FAST };

    struct WheelVelocities {
        okapi::QAngularSpeed left;
        okapi::QAngularSpeed right;
    };

    struct GeneratedProfile {
        Segment* path;
        int length;
        bool needsReversing;
    };

    using LinearAndAngular = std::tuple<okapi::QAngularSpeed, okapi::QAngularSpeed>;

    // Storage for some stuff
    std::optional<GeneratedProfile> generatedProfile = std::nullopt;
    std::optional<WaypointSet> currentPath = std::nullopt;
    std::optional<Pose> lastMeasuredPose = std::nullopt;
    std::optional<Pose> lastError = std::nullopt;
    std::optional<Pose> lastFinalTarget = std::nullopt;

    // Vector of callbacks for new target
    std::vector<std::function<void(Pose)>> newTargetCallbacks;

    // Helper functions
    okapi::QAngularSpeed linearToRotational(okapi::QSpeed linearSpeed) const;
    okapi::QAngularSpeed calculateDesiredOmega();

    // Calculate velocities with various correction types
    LinearAndAngular calculateVelocitiesRamsete();
    LinearAndAngular calculateVelocitiesHeadingCorrect();
    LinearAndAngular calculateVelocitiesBasic();
    LinearAndAngular calculateVelocities();

    // Calculate whether or not the next movement should be reversed
    bool calculateIfReversed();

    // Task trampoline and task code
    static void trampoline(void* instance);
    void taskLoop();

    // Updates the latest error
    void updateError();

    // Runs pathfinder generation
    void generateProfile();

    // Runs the path
    void executePath();

public:
    // Ctor
    ProfileController(
        std::shared_ptr<okapi::ChassisModel> model,
        std::shared_ptr<PoseEstimator> poseEstimator,
        KinematicConstraints constraints,
        okapi::ChassisScales scales,
        okapi::AbstractMotor::GearsetRatioPair ratioPair,
        RamseteGains gains,
        double headingkP);

    // okapi overrides
    WaypointSet getTarget() override;
    void setTarget(WaypointSet path) override;
    void controllerSet(WaypointSet path) override;

    // Gets the target for the end pose of current or previous movements
    Pose getFinalTarget() const;

    // gets difference between latest target and current pose
    Pose getError() const override;

    void waitUntilSettled() override;
    bool isSettled() override;

    void reset() override;
    bool isDisabled() const override;
    void flipDisable() override;
    void flipDisable(bool isDisabled) override;
    void tarePosition()override;
    WaypointSet getProcessValue() const override;
    void setMaxVelocity(std::int32_t imaxVelocity)override; 
    // Stuff for deferring

    // Sets the next movement to be deffered. I.e. it doesn't execute immediately, waits for a go command
    void deferNext();

    // Execute deffered movement
    void go();

    // Execute deffered movement and waitUntilSettled
    void goAndWait();

    // Is the next movement deffered?
    bool isDeferred();

    // Getter/setter for correction mode
    CorrectionMode getCorrectionMode();
    void setCorrectionMode(CorrectionMode mode);

    // Getter/setter for timestep
    okapi::QTime getTimestep();
    void setTimestep(okapi::QTime timestep);

    // Getter/setter for sample count
    int getSamples();
    void setSamples(int samples);

    // Getter/setter for fit mode
    FitMode getFitMode();
    void setFitMode(FitMode mode);

    // A callback for when a new target is set. This is uesful for updating a global pose in a master chassis controller or somethin
    void onNewTarget(std::function<void(Pose)> cb);

    // Movement functions
    void move(WaypointSet path);
    void move(WaypointSet path, okapi::QTime timestep);
    void moveTo(WaypointSet path);
    void moveTo(Pose pose);
    void moveForward(okapi::QLength distance);
    void moveBackward(okapi::QLength distance);

    // Async movement functions
    void moveAsync(WaypointSet path);
    void moveAsync(WaypointSet path, okapi::QTime timestep);
    void moveToAsync(WaypointSet path);
    void moveToAsync(Pose pose);
    void moveForwardAsync(okapi::QLength distance);
    void moveBackwardAsync(okapi::QLength distance); 
};
}