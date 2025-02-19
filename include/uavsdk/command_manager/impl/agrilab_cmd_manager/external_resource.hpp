#pragma once

#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/offboard/offboard.h>

#include <position_reg/positionReg.hpp>

// #include "rclcpp_action/rclcpp_action.hpp"
// #include <uav_msgs/action/planning.hpp>

#include <uavsdk/fcu_telemetry_collector/mavsdk_fcu_telemetry_collector.hpp>


namespace uavsdk
{
    namespace agrilab
    {
        class CmdExternalResources : public useful_di::TypeInterface
        {
            public:
            CmdExternalResources()
            {
                this->___set_type();
            }

            std::shared_ptr<fcu_tel_collector::TelemetryDataComposite> telem;
            std::shared_ptr<mavsdk::Action> action;
            std::shared_ptr<mavsdk::Offboard> offboard;
            std::shared_ptr<position_reg::PID> pid;
            std::shared_ptr<position_reg::PositionReg> reg;
            
            // std::shared_ptr<rclcpp_action::ServerGoalHandle<uav_msgs::action::Planning>> goal_handle;
        };
    };
};