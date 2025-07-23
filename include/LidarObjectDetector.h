#ifndef LIDAR_OBJECT_DETECTOR_H
#define LIDAR_OBJECT_DETECTOR_H

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

class LidarObjectDetector : public rclcpp::Node
{
public:
    LidarObjectDetector();

private:
    void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg);

    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
};

#endif // LIDAR_OBJECT_DETECTOR_H
