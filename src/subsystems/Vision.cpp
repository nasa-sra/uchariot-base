#include "subsystems/Vision.h"
#include "Utils.h"

#include <sys/ipc.h>
#include <sys/msg.h>

void Vision::Update(double dt)
{
}

void Vision::UpdateVisionData(std::string data)
{
    _document.Parse(data.c_str());

    _visionData.clear();

    const rapidjson::Value &arrayData = _document["detections"];

    for (rapidjson::SizeType i = 0; i < arrayData.Size(); ++i)
    {
        VisionData sVisionData;
        sVisionData.name = arrayData[i]["name"].GetString();
        sVisionData.pose = Eigen::Vector3d(arrayData[i]["x"].GetDouble(), arrayData[i]["y"].GetDouble(), arrayData[i]["z"].GetDouble());

        if (sVisionData.pose.norm() > 1.0)
        {
        }

        _visionData.push_back(sVisionData);
    }
}

void Vision::ReportState(std::string prefix)
{
    prefix += "vision/";
    StateReporter::GetInstance().UpdateKey(prefix + "heading", _heading);
}
