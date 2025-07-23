#include "LidarObjectDetector.h"
#include <limits>
#include <algorithm>

LidarObjectDetector::LidarObjectDetector()
: Node("lidar_object_detector")
{
    subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", 10,
        std::bind(&LidarObjectDetector::scan_callback, this, std::placeholders::_1)
    );
}

void LidarObjectDetector::scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
    float closest_distance = std::numeric_limits<float>::infinity();
    int index = -1;

    for (size_t i = 0; i < msg->ranges.size(); ++i)
    {
        float distance = msg->ranges[i];
        if (std::isfinite(distance) && distance < closest_distance)
        {
            closest_distance = distance;
            index = static_cast<int>(i);
        }
    }

    if (index != -1)
    {
        float angle = msg->angle_min + index * msg->angle_increment;
        RCLCPP_INFO(this->get_logger(),
                    "Closest object: %.2f m at angle %.2f radians", closest_distance, angle);
    }
    else
    {
        RCLCPP_INFO(this->get_logger(), "No valid object detected.");
    }
}
