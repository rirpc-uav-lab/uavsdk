#pragma once

#include <vector>
#include <uavsdk/data_adapters/ros2/geometry_msgs/pose.hpp>
#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>

namespace uavsdk
{
    namespace agrilab
    {
        class PathFollowingData : public useful_di::TypeInterface
        {
            public:
            PathFollowingData()
            {
                this->___set_type();
            }

            std::vector<uavsdk::data_adapters::ros2::geometry_msgs::Pose> global_trajectory_wgs84;
            float loiter_time_s;
            float acceptance_radius_m;
            float velocity;
        };


        class TryReturnToPrecLandData : public useful_di::TypeInterface
        {
            public:
            TryReturnToPrecLandData()
            {
                this->___set_type();
            }
            std::vector<uavsdk::data_adapters::ros2::geometry_msgs::Pose> land_wgs84_position;
        };


        class TakeOffData : public useful_di::TypeInterface
        {
            public:
            TakeOffData()
            {
                this->___set_type();
            }
            std::vector<uavsdk::data_adapters::ros2::geometry_msgs::Pose> height_pose;
        };


        class GoToLocationData : public useful_di::TypeInterface
        {
            public:
            GoToLocationData()
            {
                this->___set_type();
            }
            std::vector<uavsdk::data_adapters::ros2::geometry_msgs::Pose> global_location_wgs84;
        };
    };
};