#pragma once

#include <string>
#include <chrono>
#include <thread>

#include <uavsdk/command_manager/command_interface.hpp>

#include <uavsdk/command_manager/impl/agrilab_cmd_manager/external_resource.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/command_data.hpp>

#include <uavsdk/utils/conversion_functions.hpp>

// #include <geometry_msgs/msg/pose.hpp>

#include <uavsdk/data_adapters/ros2/geometry_msgs/pose.hpp>

using namespace std::chrono_literals;

namespace uavsdk
{
    namespace agrilab
    {
        namespace commands
        {
            class CheckOffboard : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, void>
            {
                public:
                CheckOffboard(std::shared_ptr<CmdExternalResources> ext_res)
                {
                    this->set_external_resource(ext_res);
                    this->set_id("check_offboard");
                }

                protected:
                void logic_tick() override
                {
                    auto landed_state = std::dynamic_pointer_cast<fcu_tel_collector::LandedStateData>(this->external_resource->telem->get_msg()->at("landed_state"));

                    if (not landed_state->initialized)
                    {
                        return;
                    }

                    if (landed_state->get_msg() != mavsdk::Telemetry::LandedState::OnGround)
                    {   
                        std::cout << "TakeOff: not on ground!";
                        std::cout << landed_state->get_data() << "\n\n";
                        // this->action
                        this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "72");
                    }

                    auto flight_mode = std::dynamic_pointer_cast<fcu_tel_collector::FlightModeData>(this->external_resource->telem->get_msg()->at("flight_mode"));

                    if (not flight_mode->initialized)
                    {
                        return;
                    }

                    auto msg = flight_mode->get_msg();
                    std::cout << "current_flight_mode = " << utils::conversions::mavsdk_ext::convert_flight_mode_to_str(msg) << "\n";

                    if (flight_mode->get_msg() == mavsdk::Telemetry::FlightMode::Offboard)
                    {
                        int i = 20;
                        auto res = this->external_resource->action->hold();
                        // auto res = action->hold();
                        while (res != mavsdk::Action::Result::Success and i--)
                        {
                            std::this_thread::sleep_for(50ms);
                            res = this->external_resource->action->hold();
                        }
                        if (i <= 0) this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "92");
                    }

                    return;
                }


                void handle_stop() override
                {
                    auto res = this->external_resource->action->hold();
                    while (res != mavsdk::Action::Result::Success)
                    {
                        res = this->external_resource->action->hold();
                    }
                }
            };


            class CheckSetpoint : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                protected:
                void logic_tick() override
                {
                    mission_item = uavsdk::data_adapters::ros2::geometry_msgs::Pose();
                    // mission_item = geometry_msgs::msg::Pose();
                    if (this->command_data->height_pose.size() > 0) 
                    {
                        auto pose_setpoint = this->command_data->height_pose.at(0);
                        mission_item.position.x = pose_setpoint.position.x; 
                        mission_item.position.y = pose_setpoint.position.y; 
                        mission_item.position.z = pose_setpoint.position.z; 
                        mission_item.orientation.x = pose_setpoint.orientation.x; 
                        mission_item.orientation.y = pose_setpoint.orientation.y; 
                        mission_item.orientation.z = pose_setpoint.orientation.z; 
                        mission_item.orientation.w = pose_setpoint.orientation.w;
                        
                        if (!mission_item.position.z > 0)
                        {
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "118");
                        }
                        #warning какую высоту максимальную можно задать при take_off или мне её Димон отправляет???? СЕЙЧАС 30 МЕТРОВ
                        if (!(z_coor <= 30))
                        {
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "123");
                        }
                    }
                    else this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "126");
                    states.erase(states.begin());
                    return;
                }
            }

            class TakeOff : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                public:
                TakeOff(std::shared_ptr<CmdExternalResources> ext_res, std::shared_ptr<TakeOffData> data, std::shared_ptr<useful_di::UniMapStr> blackboard_init)
                {
                    this->set_external_resource(ext_res);
                    this->set_command_data(data);
                    this->init_blackboard(blackboard_init);
                    this->set_id("take_off"); // path_following

                    states = {"wait_for_health", "arm", "start_takeoff", "check_position"};
                }


                protected:
                void logic_tick() override
                {
                    // std::cout << "States = \n";
                    // for (const auto& state : states)
                    // {
                    //     std::cout << state << "\n";
                    // }

                    // if (states.at(0) == "wait_for_telemetry")
                    // {
                    //     std::cout << this->get_id() << ": " << states.at(0) << "\n";
                    //     std::cout << this->external_resource->telem->all_data_initialized() << "\n";
                    //     if (this->external_resource->telem->all_data_initialized())
                    //     {
                    //         this->states.erase(states.begin());
                    //         return;
                    //     }
                    // }

                    else if (states.at(0) == "check_setpoint")
                    {}

                    else if (states.at(0) == "wait_for_health")
                    {
                        std::cout << this->get_id() << ": " << states.at(0) << "\n";
                        if (!this->external_resource->telem->health_all_ok())
                        {
                            std::cout << "Waiting for system to be ready\n";
                            // std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                        else 
                        {
                            states.erase(states.begin());
                            return;
                        }
                    }

                    else if (states.at(0) == "arm")
                    {
                        std::cout << this->get_id() << ": " << states.at(0) << "\n";
                        std::cout << "System ready\n";

                        std::cout << "Arming...\n";
                        const mavsdk::Action::Result arm_result = this->external_resource->action->arm();
                        if (arm_result != mavsdk::Action::Result::Success) 
                        {
                            std::cerr << "Arming failed: " << arm_result << '\n';
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "156");
                        }
                        else
                        {
                            states.erase(states.begin());
                        }
                        // std::cout << "Armed.\n";
                    }

                    else if (states.at(0) == "start_takeoff")
                    {
                        std::cout << this->get_id() << ": " << states.at(0) << "\n";
                        z_coor = mission_item.position.z;
                        mavsdk::Action::Result takeoff_result = this->external_resource->action->set_takeoff_altitude(z_coor);
                        takeoff_result = this->external_resource->action->takeoff();

                        if (takeoff_result != mavsdk::Action::Result::Success) 
                        {
                            std::cerr << "Takeoff failed: " << takeoff_result << '\n';
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "175");
                        }
                        else
                        {
                            target_alt = this->external_resource->action->get_takeoff_altitude().second;
                            current_height = 0;
                            states.erase(states.begin());
                            return;
                        }
                    }

                    else if (states.at(0) == "check_position")
                    {
                        std::cout << this->get_id() << ": " << states.at(0) << "\n";
                        #warning Будет ли дрон при take_off прям точно долетать до нужной точки по высоте?
                        
                        auto current_uav_position = std::dynamic_pointer_cast<fcu_tel_collector::PositionData>(this->external_resource->telem->get_msg()->at("uav_position"));

                        if (current_uav_position->get_msg().relative_altitude_m < target_alt-1) 
                        {
                            current_height = current_uav_position->get_msg().relative_altitude_m;
                            // current_height = telemetry->position().relative_altitude_m;
                            // std::cout << "TAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFF\n";
                            std::this_thread::sleep_for(100ms);                    
                        } 
                        else
                        {
                            states.erase(states.begin());
                            this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS, "203");
                            return;
                        }
                    }
                }


                void handle_stop() override
                {
                    auto res = this->external_resource->action->hold();
                    while (res != mavsdk::Action::Result::Success)
                    {
                        res = this->external_resource->action->hold();
                    }
                }


                private:
                std::vector<std::string> states; // {"check_offboard", "check_setpoint", "wait_for_health", "arm", "start_takeoff", "check_position"}

                float z_coor;
                float target_alt;
                float current_height;
                uavsdk::data_adapters::ros2::geometry_msgs::Pose mission_item;

            };
        };
    };
};