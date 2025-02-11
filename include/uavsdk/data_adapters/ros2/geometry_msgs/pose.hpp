#pragma once

#include <uavsdk/data_adapters/ros2/geometry_msgs/quaternion.hpp>
#include <uavsdk/data_adapters/ros2/geometry_msgs/point32.hpp>

namespace uavsdk
{
    namespace data_adapters
    {
        namespace ros2
        {
            namespace geometry_msgs
            {
                class Pose
                {
                    public:
                    Point32 pose;
                    Quaternion orientation;
                };
            };
        };
    };
};