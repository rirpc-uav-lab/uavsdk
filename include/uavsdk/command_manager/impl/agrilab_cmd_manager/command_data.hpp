#pragma once

#include <vector>
#include <uavsdk/data_adapters/ros2/geometry_msgs/pose.hpp>

namespace uavsdk
{
    namespace agrilab
    {
        class PathFollowingData
        {
            public:
            std::vector<uavsdk::data_adapters::ros2::geometry_msgs::Pose> global_trajectory_wgs84;
            float loiter_time_s;
            float acceptance_radius_m;
            float velocity;
        };


        class TryReturnToPrecLandData
        {
            public:
            std::vector<uavsdk::data_adapters::ros2::geometry_msgs::Pose> land_wgs84_position;
        };


        class TakeOffData
        {
            public:
            std::vector<uavsdk::data_adapters::ros2::geometry_msgs::Pose> height_pose;
        };


        class GoToLocationData
        {
            public:
            std::vector<uavsdk::data_adapters::ros2::geometry_msgs::Pose> global_location_wgs84;
        };
    };
};