#pragma once

#include <string>
#include <chrono>
#include <thread>

#include <mavsdk/plugins/telemetry/telemetry.h>

#include <uavsdk/command_manager/command_interface.hpp>

#include <uavsdk/command_manager/impl/agrilab_cmd_manager/external_resource.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/command_data.hpp>

#include <uavsdk/utils/conversion_functions.hpp>

#include <uavsdk/data_adapters/ros2/geometry_msgs/pose.hpp>
#include <uavsdk/data_adapters/cxx/cxx.hpp>

using namespace std::chrono_literals;

namespace uavsdk
{
    namespace agrilab
    {
        namespace commands
        {
            class Land : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
            {
                public:
                Land(std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_id("land");
                    this->___set_type();


                    auto land_check = std::make_shared<LandCheck>(this->get_bb_p());

                    this->add_stage(land_check);
                }

                protected:

                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    { // BLOCK: check_landed_state
                        auto landed_state = std::dynamic_pointer_cast<fcu_tel_collector::LandedStateData>(this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->telem->get_msg()->at("landed_state"));

                        if (landed_state->get_msg() == mavsdk::Telemetry::LandedState::OnGround)
                        {
                            return uavsdk::command_manager::ExecutionResult::FAILED;
                        }
                    }

                    { // BLOCK: Check mission
                        std::vector<mavsdk::Mission::MissionItem> mission_items;
            
                        for (int i = 0; i < 1; i++) std::cout << "PLD_SRCH_ALT" << PLD_SRCH_ALT << "\n";
            
                        std::string mission_source = "";
            
                        #warning must add credibility checks for the land position and remove dummy code 
                        if (land_geo_position)
                        {
                            mission_items.push_back(make_mission_item(
                                land_geo_position.value().position.x,
                                land_geo_position.value().position.y,
                                PLD_SRCH_ALT,
                                1.0f,
                                false,
                                0.0f,
                                0.0f,
                                mavsdk::Mission::MissionItem::CameraAction::None,
                                0, 
                                1.0));
                                mission_source = "waypoint";
                            mission_items.push_back(make_mission_item(
                                land_geo_position.value().position.x,
                                land_geo_position.value().position.y,
                                PLD_SRCH_ALT,
                                1.0f,
                                false,
                                0.0f,
                                0.0f,
                                mavsdk::Mission::MissionItem::CameraAction::None,
                                0, 
                                1.0));
                                mission_source = "waypoint";
                        }
                        else
                        {
                            mission_items.push_back(make_mission_item(
                                home_position.latitude_deg,
                                home_position.longitude_deg,
                                PLD_SRCH_ALT,
                                1.0f,
                                false,
                                0.0f,
                                0.0f,
                                mavsdk::Mission::MissionItem::CameraAction::None,
                                0, 
                                1.0));
                                mission_source = "home_position";
                            mission_items.push_back(make_mission_item(
                                home_position.latitude_deg,
                                home_position.longitude_deg,
                                PLD_SRCH_ALT,
                                1.0f,
                                false,
                                0.0f,
                                0.0f,
                                mavsdk::Mission::MissionItem::CameraAction::None,
                                0, 
                                1.0));
                                mission_source = "home_position";
                        }
                    }

                    { // BLOCK: fly to home
                        std::cout << "Uploading mission...\n";
                        Mission::MissionPlan mission_plan{};
                        mission_plan.mission_items = mission_items;
            
                        // mavsdk::Mission::Result upload_res = mission->clear_mission();
            
                        // while (upload_res != Mission::Result::Success) {
                        //     std::cerr << "Mission clear failed: " << upload_res << ", trying again.\n";
                        //     // upload_res = mission->clear_mission();
                        // }
                        Mission::Result upload_result = mission->upload_mission(mission_plan);
            
                        while (upload_result != Mission::Result::Success) {
                            if (stopper->stop_requested())
                            {
                                auto res = action->hold();
                                while (res != mavsdk::Action::Result::Success)
                                {
                                    std::this_thread::sleep_for(50ms);
                                    res = action->hold();
                                }
                                return uav_msgs::action::Planning::Result::RES_FAILED;
                            }
                            std::cerr << "Mission upload failed: " << upload_result << ", trying again.\n";
                            upload_result = mission->upload_mission(mission_plan);
                        }
            
                        int counter = 10;
                        uav_msgs::action::Planning_Feedback::SharedPtr fb = std::make_shared<uav_msgs::action::Planning_Feedback>();
            
                        mavsdk::Mission::Result start_mission_result = mission->start_mission();
                        while (start_mission_result != mavsdk::Mission::Result::Success and counter--) 
                        // if (start_mission_result != mavsdk::Mission::Result::Success) 
                        {
                            if (stopper->stop_requested())
                            {
                                auto res = action->hold();
                                while (res != mavsdk::Action::Result::Success)
                                {
                                    std::this_thread::sleep_for(50ms);
                                    res = action->hold();
                                }
                                return uav_msgs::action::Planning::Result::RES_FAILED;
                            }
                            std::cerr << "Starting mission failed: " << start_mission_result << '\n';
                            std::cerr << "Mission source: " << mission_source << '\n';
                            std::cerr << "home_position.latitude_deg: " << home_position.latitude_deg << '\n';
                            std::cerr << "home_position.longitude_deg: " << home_position.longitude_deg << '\n';
                            std::this_thread::sleep_for(100ms);
                            // return uav_msgs::action::Planning::Result::RES_FAILED; 
                            // return uav_msgs::action::Planning::Result::RES_FAILED; 
                            // return uav_msgs::action::Planning::Result::RES_FAILED; 
                            // return uav_msgs::action::Planning::Result::RES_FAILED; 
                            // return uav_msgs::action::Planning::Result::RES_FAILED; 
                            #warning Wrong logic! prec land
                        }    
                        
                        std::shared_ptr<std::atomic<bool>> mission_complete = std::make_shared<std::atomic<bool>>(false);
                        // Before starting the mission_raw, we want to be sure to subscribe to the mission_raw progress.
                        
                        auto ms_handle = mission->subscribe_mission_progress([mission_complete](mavsdk::Mission::MissionProgress mission_progress) 
                        {
                            std::cout << "Mission status update: " << mission_progress.current << " / "
                                    << mission_progress.total << '\n';
                            if (mission_progress.current == mission_progress.total && mission_progress.total != 0)
                            {
                                // We can only set a flag here. If we do more request inside the callback,
                                // we risk blocking the system.
            
                                mission_complete->store(true);
                            }
                        });
            
                        while (not mission_complete->load())
                        {
                            if (stopper->stop_requested())
                            {
                                auto res = action->hold();
                                while (res != mavsdk::Action::Result::Success)
                                {
                                    std::this_thread::sleep_for(50ms);
                                    res = action->hold();
                                }
                                return uav_msgs::action::Planning::Result::RES_FAILED;
                            }
                            std::this_thread::sleep_for(100ms);
                        }
                        
                        mission->unsubscribe_mission_progress(ms_handle);
                    }

                    { // BLOCK: Search marker
                        double search_start_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                        double current_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
            
                        #warning result is not used!
                        offboard->set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
            
                        mavsdk::Offboard::Result offboard_result = offboard->start();
                        while (offboard_result != mavsdk::Offboard::Result::Success) 
                        {
                            if (stopper->stop_requested())
                            {
                                auto res = action->hold();
                                while (res != mavsdk::Action::Result::Success)
                                {
                                    std::this_thread::sleep_for(50ms);
                                    res = action->hold();
                                }
            
                                // mavsdk::Offboard::Result offboard_res = offboard->stop();
            
                                // while (offboard_res != mavsdk::Offboard::Result::Success) 
                                // {
                                //     RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << offboard_res << '\n');
                                //     offboard_res = offboard->stop();
                                //     std::this_thread::sleep_for(50ms);
                                // }
                                return uav_msgs::action::Planning::Result::RES_FAILED;
                            }
                            std::cerr << "Offboard::start() failed: " << offboard_result << '\n';
                            std::this_thread::sleep_for(20ms);
                            offboard_result = offboard->start();
                        }
            
                        bool search_timed_out = false;
                        bool marker_visible = true;
            
                        auto search_for_marker_func = [
                                this, 
                                &current_time, 
                                &marker_visible, 
                                &search_start_time, 
                                &search_timed_out,
                                &stopper
                            ]
                            ()
                        {
                            for (int i = 0; i < 1; i++) std::cout << "Marker not visible. Searching...\n";
                            current_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                            marker_visible = (current_time - last_time_marker_was_seen) < PLD_MARKER_TIMEOUT_S;
            
                            search_timed_out = ((current_time - search_start_time) > PLD_SRCH_TIMEOUT);
                            if (search_timed_out)
                            {
                                for (int i = 0; i < 1; i++) std::cout << "Search timed out.\n";
                                Action::Result land_result = action->land();
                                while (land_result != Action::Result::Success) {
                                    if (stopper->stop_requested())
                                    {
                                        // mavsdk::Offboard::Result offboard_res = offboard->stop();
            
                                        // if (offboard_res != mavsdk::Offboard::Result::Success) 
                                        // {
                                        //     RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << offboard_res << '\n');
                                        //     offboard_res = offboard->stop();
                                        //     std::this_thread::sleep_for(50ms);
                                        // }
                                        auto res = action->hold();
                                        if (res != mavsdk::Action::Result::Success)
                                        {
                                            std::this_thread::sleep_for(50ms);
                                            res = action->hold();
                                        }
                                        return uav_msgs::action::Planning::Result::RES_FAILED;
                                    }
                                    //Land failed, so exit (in reality might try a return to land or kill.)
                                    // return 1;
                                    #warning blockage threat!
                                    land_result = action->land();
                                }
            
                                while (current_landed_state != mavsdk::Telemetry::LandedState::OnGround)
                                {
                                    if (stopper->stop_requested())
                                    {
                                        // mavsdk::Offboard::Result offboard_res = offboard->stop();
            
                                        // while (offboard_res != mavsdk::Offboard::Result::Success) 
                                        // {
                                        //     RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << offboard_res << '\n');
                                        //     offboard_res = offboard->stop();
                                        //     std::this_thread::sleep_for(50ms);
                                        // }
                                        auto res = action->hold();
                                        while (res != mavsdk::Action::Result::Success)
                                        {
                                            std::this_thread::sleep_for(50ms);
                                            res = action->hold();
                                        }
                                        return uav_msgs::action::Planning::Result::RES_FAILED;
                                    }
                                    std::this_thread::sleep_for(100ms);
                                }
                                return uav_msgs::action::Planning::Result::RES_SUCCESS;
                            }
                            return uint8_t(255);
                        };
            
                        while (!search_timed_out)
                        {
                            if (stopper->stop_requested())
                            {
                                auto res = action->hold();
                                while (res != mavsdk::Action::Result::Success)
                                {
                                    std::this_thread::sleep_for(50ms);
                                    res = action->hold();
                                }
            
                                return uav_msgs::action::Planning::Result::RES_FAILED;
                            }
                            if (!marker_visible)
                            {
                                uint8_t res = search_for_marker_func();
                                if (res != 255)
                                {
                                    return res;
                                }
                            }
                            else
                            {
                                std::cout << "Marker found\n";
                                search_start_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                                
                                break;
                            }
                            std::this_thread::sleep_for(100ms);
                        }
                    }

                    { // BLOCK: helper funcs
                        // main prec land loop
                        pid_regulator::PID pid(PLD_KP, PLD_KI, PLD_KD);
                        pid_regulator::PID pid_yaw(0.5, 0.0, 0.0);
                        pid_regulator::PID pid_z(0.3, 0.0, 0.1);
                        double last_pid_time = 0;
            
                        float dr = sqrt(x_rad * x_rad + y_rad * y_rad);
            
                        auto regulate_position_func = [
                            this,
                            &dr,
                            &current_time,
                            &last_pid_time,
                            &pid
                        ]
                        ()
                        {
                            dr = sqrt(x_rad * x_rad + y_rad * y_rad);
            
                            float movement_radius = pid.pid(dr, current_time - last_pid_time);
                            float alpha = atan2(-y_rad, x_rad);
            
                            float x_speed = movement_radius * cos(alpha);
                            float y_speed = movement_radius * sin(alpha);
            
                            float vr = sqrt(x_speed * x_speed + y_speed * y_speed);
                            if (vr > PLD_MAX_SPD)
                            {
                                x_speed = PLD_MAX_SPD * cos(alpha);
                                y_speed = PLD_MAX_SPD * sin(alpha);
                            }
            
                            return std::make_tuple(x_speed, y_speed, alpha);
                        };
            
            
                        auto regulate_height_func = [
                            this,
                            &current_time,
                            &last_pid_time,
                            &pid_z
                        ]
                        (float goal_height, float min_speed)
                        {
                            // float z_speed = PLD_MIN_ALT_SPD;
                            // if (current_distance_sensor_data.current_distance_m < PLD_SRCH_ALT) z_speed = make_negative(z_speed);
            
                            std_msgs::msg::String msg;
                            msg.data = "goal_height: " + std::to_string(goal_height);
            
                            info_pub->publish(msg);
                            float error = goal_height - current_distance_sensor_data.current_distance_m;
                            
                            msg.data = "current_distance_sensor_data: " + std::to_string(current_distance_sensor_data.current_distance_m);
            
                            info_pub->publish(msg);
                            float z_speed = pid_z.pid(error, current_time - last_pid_time);
                            
                            msg.data = "current_time - last_pid_time: " + std::to_string(current_time - last_pid_time);
            
                            info_pub->publish(msg);
            
                            msg.data = "z_speed after pid: " + std::to_string(z_speed);
            
                            info_pub->publish(msg);
                            if (abs(z_speed) > PLD_MAX_ALT_SPD) z_speed = sgn(z_speed) * PLD_MAX_ALT_SPD;
                            if (abs(z_speed) < PLD_MIN_ALT_SPD) z_speed = sgn(z_speed) * PLD_MIN_ALT_SPD;
            
                            msg.data = "z_speed after min max: " + std::to_string(z_speed);
            
                            info_pub->publish(msg);
            
                            return -z_speed;
                        };
                    }
                    
                    { // BLOCK: Center above marker
                        while (x_rad > (PLD_ACC_RADIUS_DEG * M_PI / 180.0) or y_rad > (PLD_ACC_RADIUS_DEG * M_PI / 180.0))
                        // while (x_rad > (PLD_ACC_RADIUS_DEG * M_PI / 180.0) or y_rad > (PLD_ACC_RADIUS_DEG * M_PI / 180.0))
                        {
                            // for (int i = 0; i < 1; i++) std::cout << "x_rad < (PLD_ACC_RADIUS_DEG * M_PI / 180.0) = " << bool(x_rad < (PLD_ACC_RADIUS_DEG * M_PI / 180.0));
                            // for (int i = 0; i < 1; i++) std::cout << "y_rad < (PLD_ACC_RADIUS_DEG * M_PI / 180.0) = " << bool(y_rad < (PLD_ACC_RADIUS_DEG * M_PI / 180.0));
                            if (stopper->stop_requested())
                            {
                                auto res = action->hold();
                                while (res != mavsdk::Action::Result::Success)
                                {
                                    std::this_thread::sleep_for(50ms);
                                    res = action->hold();
                                }
            
                                return uav_msgs::action::Planning::Result::RES_FAILED;
                            }
            
                            for (int i = 0; i < 1; i++) std::cout << "Precision loiter on...\n";
                            current_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                            marker_visible = (current_time - last_time_marker_was_seen) < PLD_MARKER_TIMEOUT_S;
                            search_timed_out = ((current_time - search_start_time) > PLD_SRCH_TIMEOUT);
            
                            if (marker_visible)
                            {
                                search_start_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                                std::tuple<float, float, float> res = regulate_position_func();
                                float x_speed = std::get<0>(res);
                                float y_speed = std::get<1>(res);
                                float dalpha = std::get<2>(res);
            
                                for (int i = 0; i < 1; i++) 
                                {
                                    std::cout << "Marker visible, regulating...\n";
                                }
                                #warning result is not used!
                                
                                // float z_speed = -pid_z.pid(PLD_SRCH_ALT - current_distance_sensor_data.current_distance_m, current_time - last_pid_time);
                                // if (abs(z_speed) < PLD_MIN_ALT_SPD) z_speed = sgn(z_speed) * PLD_MIN_ALT_SPD;
                                // if (abs(z_speed) > PLD_MAX_ALT_SPD) z_speed = sgn(z_speed) * PLD_MAX_ALT_SPD;
            
                                float z_speed = regulate_height_func(PLD_SRCH_ALT, 0.0);
                                last_pid_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
            
                                // float yaw_speed = 180.0 * pid_yaw.pid(dalpha, current_time - last_pid_time) / M_PI;
            
                                offboard->set_velocity_body({x_speed, y_speed, z_speed, 0.0f});
                            }
                            else
                            {
                                for (int i = 0; i < 1; i++) std::cout << "Marker lost.\n";
            
                                if (search_timed_out)
                                {
                                    for (int i = 0; i < 1; i++) std::cout << "Search timed out.\n";
                                    Action::Result land_result = action->land();
                                    while (land_result != Action::Result::Success) {
                                        //Land failed, so exit (in reality might try a return to land or kill.)
                                        // return 1;
                                        #warning blockage threat!
                                        land_result = action->land();
                                    }
            
                                    while (current_landed_state != mavsdk::Telemetry::LandedState::OnGround)
                                    {
                                        std::this_thread::sleep_for(100ms);
                                    }
            
                                    int i = 0;
                                    while (action->hold() != mavsdk::Action::Result::Success)
                                    {
                                        if (i > 10) return uav_msgs::action::Planning::Result::RES_FAILED;
                                        std::this_thread::sleep_for(100ms);
                                        i++;
                                    }
                                    return uav_msgs::action::Planning::Result::RES_SUCCESS;
                                }
                                else
                                {
                                    for (int i = 0; i < 1; i++) std::cout << "Searching for marker...\n";
                                    offboard->set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
                                }
                            }
                            std::this_thread::sleep_for(100ms);
                        }
                    }
                    { // BLOCK: orientation regulation
                        while (marker_visible and abs(alpha_rad) > 10.0 * M_PI / 180.0)
                        {
                            search_start_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                            if (stopper->stop_requested())
                            {
                                auto res = action->hold();
                                while (res != mavsdk::Action::Result::Success)
                                {
                                    std::this_thread::sleep_for(50ms);
                                    res = action->hold();
                                }
            
                                return uav_msgs::action::Planning::Result::RES_FAILED;
                            }
                            current_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                            marker_visible = (current_time - last_time_marker_was_seen) < PLD_MARKER_TIMEOUT_S;
                            search_timed_out = ((current_time - search_start_time) > PLD_SRCH_TIMEOUT);
                            float alpha = atan2(-y_rad, x_rad);
            
                            std::tuple<float, float, float> res = regulate_position_func();
                            float x_speed = std::get<0>(res);
                            float y_speed = std::get<1>(res);
                            float dalpha = std::get<2>(res);
            
                            float z_speed = regulate_height_func(PLD_SRCH_ALT, 0.0);
            
                            if (abs(alpha_rad) > 10.0 * M_PI / 180.0)
                            {
                                float yaw_speed = 180.0 * pid_yaw.pid(alpha_rad, current_time - last_pid_time) / M_PI;
                                offboard->set_velocity_body({x_speed, y_speed, z_speed, -yaw_speed});
                            }
                            else
                            {
                                break;
                            }
                            last_pid_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                            std::this_thread::sleep_for(100ms);
                        }
                    }
                    { //  BLOCK: Check search timeout
                        if (search_timed_out)
                        {
                            for (int i = 0; i < 1; i++) std::cout << "Search timed out.\n";
                            Action::Result land_result = action->land();
                            while (land_result != Action::Result::Success) {
                                //Land failed, so exit (in reality might try a return to land or kill.)
                                // return 1;
                                #warning blockage threat!
                                land_result = action->land();
                            }
            
                            while (current_landed_state != mavsdk::Telemetry::LandedState::OnGround)
                            {
                                std::this_thread::sleep_for(100ms);
                            }
                            return uav_msgs::action::Planning::Result::RES_SUCCESS;
                        }
                    }
                    
                    { // BLOCK: Landing
                        #warning z_speed is not regulated according to alittude!
                        while (current_landed_state != mavsdk::Telemetry::LandedState::OnGround)
                        {
                            for (int i = 0; i < 1; i++) std::cout << "Entered main prec land loop\n";
                            
                            { // BLOCK: variable checks
                                search_timed_out = ((current_time - search_start_time) > PLD_SRCH_TIMEOUT);
                                current_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                                marker_visible = (current_time - last_time_marker_was_seen) < PLD_MARKER_TIMEOUT_S;
                            }
                            
                            if (marker_visible) // BLOCK: marker visible
                            {
                                search_start_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                                std::tuple<float, float, float> res = regulate_position_func(); // BLOCK: 
                                float x_speed = std::get<0>(res);
                                float y_speed = std::get<1>(res);
                                float dalpha = alpha_rad;
            
                                float z_speed = regulate_height_func(0.0, PLD_MIN_ALT_SPD); // BLOCK: 
            
                                for (int i = 0; i < 1; i++) 
                                {
                                    std::cout << "Marker visible, regulating...\n";
                                }
                                #warning result is not used!
                                float yaw_speed = 180.0 * pid_yaw.pid(dalpha, current_time - last_pid_time) / M_PI; // BLOCK: regulate yaw
            
                                last_pid_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                                offboard->set_velocity_body({x_speed, y_speed, z_speed, -yaw_speed});
                            }
                            
                            else // BLOCK: marker not visible
                            {
                                for (int i = 0; i < 1; i++) std::cout << "Marker not visible...\n";
                                // x_pid.clear();
                                // y_pid.clear();
                                pid.clear();
                                
                                if (search_timed_out) // BLOCK: search timed out 
                                {
                                    { // BLOCK: Land
                                        mavsdk::Offboard::Result offboard_res = offboard->stop();
                                            while (offboard_res != mavsdk::Offboard::Result::Success) 
                                            {
                                                RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << offboard_res << '\n');
                                                offboard_res = offboard->stop();
                                                std::this_thread::sleep_for(50ms);
                                            }
                    
                                            auto res = action->hold();
                                            while (res != mavsdk::Action::Result::Success)
                                            {
                                                std::this_thread::sleep_for(50ms);
                                                res = action->hold();
                                            }

                                        Action::Result land_result = action->land();
                                        while (land_result != Action::Result::Success) 
                                        {
                                            if (stopper->stop_requested())
                                            {
                                                // mavsdk::Offboard::Result offboard_res = offboard->stop();
                
                                                // while (offboard_res != mavsdk::Offboard::Result::Success) 
                                                // {
                                                //     RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << offboard_res << '\n');
                                                //     offboard_res = offboard->stop();
                                                //     std::this_thread::sleep_for(50ms);
                                                // }
                                                auto res = action->hold();
                                                while (res != mavsdk::Action::Result::Success)
                                                {
                                                    std::this_thread::sleep_for(50ms);
                                                    res = action->hold();
                                                }
                                                return uav_msgs::action::Planning::Result::RES_FAILED;
                                            }
                                            //Land failed, so exit (in reality might try a return to land or kill.)
                                            // return 1;
                                            #warning blockage threat!
                                            land_result = action->land();
                                        }
                                        
                                        while (current_landed_state != mavsdk::Telemetry::LandedState::OnGround)
                                        {
                                            if (stopper->stop_requested())
                                            {
                                                // mavsdk::Offboard::Result offboard_res = offboard->stop();
                
                                                // while (offboard_res != mavsdk::Offboard::Result::Success) 
                                                // {
                                                //     RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << offboard_res << '\n');
                                                //     offboard_res = offboard->stop();
                                                //     std::this_thread::sleep_for(50ms);
                                                // }
                                                auto res = action->hold();
                                                while (res != mavsdk::Action::Result::Success)
                                                {
                                                    std::this_thread::sleep_for(50ms);
                                                    res = action->hold();
                                                }
                                                return uav_msgs::action::Planning::Result::RES_FAILED;
                                            }
                                            std::this_thread::sleep_for(100ms);
                                        }
                                        return uav_msgs::action::Planning::Result::RES_SUCCESS;
                                    }
                                }
                                
                                bool uav_above_land_gurantee_threshold = (current_distance_sensor_data.current_distance_m - PLD_FIN_APPR_ALT) > 0;
                                if (uav_above_land_gurantee_threshold) // BLOCK: Check if uav above land guarantee thresh
                                {
                                    // for (int i = 0; i < 1; i++) std::cout << "Going up...\n";
                                    bool uav_lower_than_search_altitude = (PLD_SRCH_ALT - current_distance_sensor_data.current_distance_m) > 0; // BLOCK: Check if uav lower than search alt
                                    if (uav_lower_than_search_altitude) // BLOCK: go up
                                    {
                                        for (int i = 0; i < 1; i++) std::cout << "Going up...\n";
                                        #warning result is not used!
                                        float z_speed = regulate_height_func(PLD_SRCH_ALT, 0.0);
                                        auto res = offboard->set_velocity_body({0.0f, 0.0f, z_speed, 0.0f});
                                    }
                                    else // BLOCK: Go down
                                    {
                                        for (int i = 0; i < 1; i++) std::cout << "Descending...\n";
                                        float z_speed = regulate_height_func(0.0, PLD_MIN_ALT_SPD);
                                        auto res = offboard->set_velocity_body({0.0f, 0.0f, z_speed, 0.0f});
                                        #warning missing logic
                                        // go home
                                        // (delete vscode)
                                        // land in position
                                    }
                                    last_pid_time = this->get_clock()->now().seconds() + this->get_clock()->now().nanoseconds() / double(1e9);
                                }
                                else
                                {
                                    { // BLOCK: Finish landing
                                        Action::Result land_result = action->land();
                                        while (land_result != Action::Result::Success) {
                                            if (stopper->stop_requested())
                                            {
                                                auto res = action->hold();
                                                while (res != mavsdk::Action::Result::Success)
                                                {
                                                    std::this_thread::sleep_for(50ms);
                                                    res = action->hold();
                                                }
                                                return uav_msgs::action::Planning::Result::RES_FAILED;
                                            }
                                            //Land failed, so exit (in reality might try a return to land or kill.)
                                            // return 1;
                                            #warning blockage threat!
                                            land_result = action->land();
                                        }
                
                                        while (current_landed_state != mavsdk::Telemetry::LandedState::OnGround)
                                        {
                                            if (stopper->stop_requested())
                                            {
                                                // mavsdk::Offboard::Result offboard_res = offboard->stop();
                
                                                // while (offboard_res != mavsdk::Offboard::Result::Success) 
                                                // {
                                                //     RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << offboard_res << '\n');
                                                //     offboard_res = offboard->stop();
                                                //     std::this_thread::sleep_for(50ms);
                                                // }
                                                auto res = action->hold();
                                                while (res != mavsdk::Action::Result::Success)
                                                {
                                                    std::this_thread::sleep_for(50ms);
                                                    res = action->hold();
                                                }
                                                return uav_msgs::action::Planning::Result::RES_FAILED;
                                            }
                                            std::this_thread::sleep_for(100ms);
                                        }
                                        return uav_msgs::action::Planning::Result::RES_SUCCESS;
                                    }
                                }
                            }
                            std::this_thread::sleep_for(100ms);
                        }
                    }
                    // while (current_landed_state != mavsdk::Telemetry::LandedState::OnGround)
                    // {
                    // }
                    
                    // mavsdk::Offboard::Result res = offboard->stop();
        
                    // while (res != mavsdk::Offboard::Result::Success) 
                    // {
                    //     RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << res << '\n');
                    //     res = offboard->stop();
                    // }
        
                    auto hold_res = action->hold();
                    while (hold_res != mavsdk::Action::Result::Success)
                    {
                        std::this_thread::sleep_for(50ms);
                        hold_res = action->hold();
                    }
        
                    mavsdk::Action::Result res_action = action->land();
        
                    while (res_action != mavsdk::Action::Result::Success) 
                    {
                        RCLCPP_ERROR_STREAM(this->get_logger(), "Action::land() failed: " << res_action << '\n');
                        res_action = action->land();
                    }
        
                    return uav_msgs::action::Planning::Result::RES_SUCCESS;
                }

                void handle_stop() override
                {
                    
                };
            };     
        };
    };
};

