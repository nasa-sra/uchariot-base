
#include <chrono>
#include <thread>
#include <cstdio>

#include "Robot.h"

Robot::Robot() :
    _leftFront(1),
    _rightFront(4),
    _leftBack(2),
    _rightBack(3)
{
    printf("Created Robot\r\n");
}

void Robot::run(int rate, bool& running) {

    printf("Robot running\r\n");

    std::chrono::time_point startTime = std::chrono::high_resolution_clock::now();
    while(running) {
        startTime = std::chrono::high_resolution_clock::now();

        // _leftFront.setCmd(0.1);
        // _leftBack.setCmd(0.1);
        // _rightFront.setCmd(0.1);
        // _rightBack.setCmd(0.1);

        switch(_driveMode){
            case DRIVE_FORWARD:
                drive(0.1, 0.1);
                break;
            case DRIVE_BACK:
                drive(-0.1, -0.1);
                break;
            case DRIVE_LEFT:
                drive(-0.1, 0.1);
                break;
            case DRIVE_RIGHT:
                drive(0.1, -0.1);
                break;
            default:
                drive(0.0, 0.0);
        }

        int dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
        if (dt < 1000 / rate) {
            std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0 / rate - dt)));
        } else {
            printf("Robot overrun - %i ms\r\n", dt);
        }
    }

    _leftFront.setCmd(0.0);
    _rightFront.setCmd(0.0);
    _leftBack.setCmd(0.0);
    _rightBack.setCmd(0.0);

}

void Robot::changeDriveMode(const char *mode){
    if(mode == "drive_f"){
        _driveMode = DRIVE_FORWARD;
    }
    else if(mode == "drive_b"){
        _driveMode = DRIVE_BACK;
    }
    else if(mode == "drive_l"){
        _driveMode = DRIVE_LEFT;
    }
    else if(mode == "drive_r"){
        _driveMode = DRIVE_RIGHT;
    }
    else{
        _driveMode = STOP;
    }
}

void Robot::drive(float leftSpeed, float rightSpeed){
    _leftFront.setCmd(leftSpeed);
    _leftBack.setCmd(leftSpeed);
    _rightFront.setCmd(rightSpeed);
    _rightBack.setCmd(rightSpeed);
}
