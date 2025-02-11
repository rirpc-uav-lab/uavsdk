#pragma once

#include <string>
#include <chrono>
#include <thread>

#include <uavsdk/command_manager/command_interface.hpp>

#include <uavsdk/command_manager/impl/agrilab_cmd_manager/external_resource.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/command_data.hpp>

#include <geometry_msgs/msg/pose.hpp>

using namespace std::chrono_literals;

namespace uavsdk
{
    namespace agrilab
    {
        namespace commands
        {
            class TakeOff : public uavsdk::command_manager::CommandInterface<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                public:
                TakeOff(std::shared_ptr<CmdExternalResources> ext_res, std::shared_ptr<TakeOffData> data)
                {
                    this->set_external_resource(ext_res);
                    this->set_command_data(data);
                    this->set_id("take_off");

                    states = {"check_offboard", "check_setpoint", "wait_for_health", "arm", "start_takeoff", "check_position"};
                }


                protected:
                void logic_tick() override
                {
                    if (states.at(0) == "check_offboard")
                    {
                        if (std::dynamic_pointer_cast<fcu_tel_collector::LandedStateData>(this->external_resource->telem->get_msg()->at("landed_state"))->get_msg() != mavsdk::Telemetry::LandedState::OnGround)
                        {   
                            // this->action
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED);
                        }

                        if (std::dynamic_pointer_cast<fcu_tel_collector::FlightModeData>(this->external_resource->telem->get_msg()->at("flight_mode"))->get_msg() == mavsdk::Telemetry::FlightMode::Offboard)
                        {
                            int i = 20;
                            auto res = this->external_resource->action->hold();
                            // auto res = action->hold();
                            while (res != mavsdk::Action::Result::Success and i--)
                            {
                                std::this_thread::sleep_for(50ms);
                                res = this->external_resource->action->hold();
                            }
                            if (i <= 0) this->stop(uavsdk::command_manager::ExecutionResult::FAILED);
                        }

                        states.erase(states.begin());
                    }


                    else if (states.at(0) == "check_setpoint")
                    {
                        mission_item = geometry_msgs::msg::Pose();
                        if (this->command_data->height_pose.size() > 0) 
                        {
                            auto pose_setpoint = this->command_data->height_pose.at(0);
                            mission_item.position.x = pose_setpoint.pose.x; 
                            mission_item.position.y = pose_setpoint.pose.y; 
                            mission_item.position.z = pose_setpoint.pose.z; 
                            mission_item.orientation.x = pose_setpoint.orientation.x; 
                            mission_item.orientation.y = pose_setpoint.orientation.y; 
                            mission_item.orientation.z = pose_setpoint.orientation.z; 
                            mission_item.orientation.w = pose_setpoint.orientation.w;
                            
                            if (!mission_item.position.z > 0)
                            {
                                this->stop(uavsdk::command_manager::ExecutionResult::FAILED);
                            }
                            #warning какую высоту максимальную можно задать при take_off или мне её Димон отправляет????
                            if (!z_coor <= 30)
                            {
                                this->stop(uavsdk::command_manager::ExecutionResult::FAILED);
                            }
                        }
                        else this->stop(uavsdk::command_manager::ExecutionResult::FAILED);
                        states.erase(states.begin());
                    }

                    else if (states.at(0) == "wait_for_health")
                    {
                        if (!this->external_resource->telem->health_all_ok())
                        {
                            std::cout << "Waiting for system to be ready\n";
                            // std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                        else 
                        {
                            states.erase(states.begin());
                        }
                    }

                    else if (states.at(0) == "arm")
                    {
                        std::cout << "System ready\n";

                        std::cout << "Arming...\n";
                        const mavsdk::Action::Result arm_result = this->external_resource->action->arm();
                        if (arm_result != mavsdk::Action::Result::Success) 
                        {
                            std::cerr << "Arming failed: " << arm_result << '\n';
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED);
                        }
                        else
                        {
                            states.erase(states.begin());
                        }
                        // std::cout << "Armed.\n";
                    }

                    else if (states.at(0) == "start_takeoff")
                    {
                        z_coor = mission_item.position.z;
                        mavsdk::Action::Result takeoff_result = this->external_resource->action->set_takeoff_altitude(z_coor);
                        takeoff_result = this->external_resource->action->takeoff();

                        if (takeoff_result != mavsdk::Action::Result::Success) 
                        {
                            std::cerr << "Takeoff failed: " << takeoff_result << '\n';
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED);
                        }
                        else
                        {
                            target_alt = this->external_resource->action->get_takeoff_altitude().second;
                            current_position = 0;
                            states.erase(states.begin());
                        }
                    }

                    else if (states.at(0) == "check_position")
                    {
                        #warning Будет ли дрон при take_off прям точно долетать до нужной точки по высоте?
                        if (current_position < target_alt-1) 
                        {
                            current_position = std::dynamic_pointer_cast<fcu_tel_collector::PositionData>(this->external_resource->telem->get_msg()->at("uav_position"))->get_msg().relative_altitude_m;
                            // current_position = telemetry->position().relative_altitude_m;
                            std::cout << "TAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFF\n";
                            std::this_thread::sleep_for(100ms);                    
                        } 
                        else
                        {
                            states.erase(states.begin());
                            this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS);
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
                float current_position;
                geometry_msgs::msg::Pose mission_item;

            };
        };
    };
};