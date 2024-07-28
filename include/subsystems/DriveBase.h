#pragma once

#include "StateReporter.h"
#include "VescController.h"
#include "subsystems/SubsystemBase.h"

#define ROBOT_WIDTH 1.0      // m (yes really)
#define MAX_DRIVE_SPEED 5.0  // m/s

struct DriveBaseCmds {
    double velocity;
    double angularVelocity;

    void ReportState(std::string prefix = "/");
};

struct DriveBaseFeedback {
    double lf, rf, lb, rb;
};

class DriveBase : public SubsystemBase {
   public:
    DriveBase();

    void Update(double dt) override;

    void ReportState(std::string prefix = "/") override;
    DriveBaseFeedback GetVelocities();
    inline void SetCmds(DriveBaseCmds cmds) { _cmds = cmds; }

    // void SetObstacleDetected(bool obstacleDetected) {
    //     _obstacleDetected = obstacleDetected;
    //     Utils::LogFmt("Obstacle detected: %s",
    //                   obstacleDetected ? "true" : "false");
    // }

   private:
    DriveBaseCmds _cmds;

    VescController _left_front;
    VescController _right_front;
    VescController _left_back;
    VescController _right_back;

    double _voltage;
};