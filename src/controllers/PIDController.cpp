#include "controllers/PIDController.h"

double PIDController::Calculate(double current, double target) {
    return _p * (target - current);
}