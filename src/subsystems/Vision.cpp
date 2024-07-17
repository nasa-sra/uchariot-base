#include "subsystems/Vision.h"
#include "Utils.h"

#include <sys/ipc.h>
#include <sys/msg.h>

Vision::Vision() {

}

double Vision::GetEntry(const std::string& entry)
{
    // Grab the IMU message super easy
    key_t key = ftok(("tmp/" + entry).c_str(), 65);
    int msg_id = msgget(key, 0666 | IPC_CREAT);
    msgrcv(msg_id, &_msg, sizeof(_msg), 1, IPC_NOWAIT);
    msgctl(msg_id, IPC_RMID, NULL);

    try {
        return std::stod(_msg._content);
    } catch (const std::exception& e) {
        // Log error and return default value
        Utils::ErrFmt("Couldn't parse entry \"%s\" with value \"%s\".", entry, _msg._content);
        return 0.0;  // Return default value or throw an exception?
    }
}

void Vision::Update(double dt) {
    _heading = GetEntry("rs_heading");
    //Utils::LogFmt("Heading = %f", _heading);
}

void Vision::ReportState(std::string prefix) {
    prefix += "vision/";
    StateReporter::GetInstance().UpdateKey(prefix + "heading", _heading);
}
