#pragma once

#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>

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
                class Pose : useful_di::TypeInterface
                {
                    public:
                    Point32 position;
                    Quaternion orientation;

                    protected:
                    virtual void ___set_type()
                    {

                    }
                };
            };
        };
    };
};