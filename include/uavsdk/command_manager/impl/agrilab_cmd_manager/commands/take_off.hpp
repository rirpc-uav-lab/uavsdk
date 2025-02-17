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
#include <uavsdk/data_adapters/cxx/cxx.hpp>

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
                CheckOffboard(std::shared_ptr<CmdExternalResources> ext_res, std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_external_resource(ext_res);
                    this->set_id("check_offboard");
                }

                protected:
                void logic_tick() override
                {
                    // std::cout << this->get_id() << "::tick()\n";
                    auto landed_state = std::dynamic_pointer_cast<fcu_tel_collector::LandedStateData>(this->external_resource->telem->get_msg()->at("landed_state"));

                    if (not landed_state->initialized)
                    {
                        return;
                    }

                    if (landed_state->get_msg() != mavsdk::Telemetry::LandedState::OnGround)
                    {   
                        // std::cout << "TakeOff: not on ground!";
                        // std::cout << landed_state->get_data() << "\n\n";
                        // this->action
                        this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "72");
                        return;
                    }

                    auto flight_mode = std::dynamic_pointer_cast<fcu_tel_collector::FlightModeData>(this->external_resource->telem->get_msg()->at("flight_mode"));

                    if (not flight_mode->initialized)
                    {
                        return;
                    }

                    auto msg = flight_mode->get_msg();
                    // std::cout << "current_flight_mode = " << utils::conversions::mavsdk_ext::convert_flight_mode_to_str(msg) << "\n";

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
                        return;
                    }

                    this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS);
                    return;
                }


                void handle_stop() override
                {
                    // auto res = this->external_resource->action->hold();
                    // while (res != mavsdk::Action::Result::Success)
                    // {
                    //     res = this->external_resource->action->hold();
                    // }
                }
            };


            class CheckSetpoint : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                public:
                CheckSetpoint(std::shared_ptr<uavsdk::agrilab::CmdExternalResources> ext_res, std::shared_ptr<uavsdk::agrilab::TakeOffData> data, std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_external_resource(ext_res);
                    this->set_command_data(data);
                    this->set_id("check_setpoint");
                }


                protected:
                void logic_tick() override
                {
                    // std::cout << this->get_id() << "::tick()\n";
                    //  mission_item uavsdk::data_adapters::ros2::geometry_msgs::Pose();
                    uavsdk::data_adapters::ros2::geometry_msgs::Pose mission_item = this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::data_adapters::ros2::geometry_msgs::Pose>>("mission_item")->get_data(); 
                    //  mission_item = uavsdk::data_adapters::ros2::geometry_msgs::Pose();
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
                        
                        this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::data_adapters::ros2::geometry_msgs::Pose>>("mission_item")->set_data(mission_item);

                        // this->modify_data_on_blackboard("mission_item", new_data);

                        if (!(this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::data_adapters::ros2::geometry_msgs::Pose>>("mission_item")->get_data().position.z > 0))
                        {
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "118");
                            return;
                        }
                        #warning какую высоту максимальную можно задать при take_off или мне её Димон отправляет???? СЕЙЧАС 30 МЕТРОВ
                        if (!(this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::data_adapters::ros2::geometry_msgs::Pose>>("mission_item")->get_data().position.z <= 30))
                        {
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "123");
                            return;
                    }
                    }
                    else
                    {
                        this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "126");
                        return;
                    }
                    this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS);
                    return;
                }


                void handle_stop() override
                {
                    // auto res = this->external_resource->action->hold();
                    // while (res != mavsdk::Action::Result::Success)
                    // {
                    //     res = this->external_resource->action->hold();
                    // }
                }
            };


            class WaitForHealth : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                public:
                WaitForHealth(std::shared_ptr<uavsdk::agrilab::CmdExternalResources> ext_res, std::shared_ptr<uavsdk::agrilab::TakeOffData> data, std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_external_resource(ext_res);
                    this->set_command_data(data);
                    this->set_id("wait_for_health");
                }


                protected:
                void logic_tick() override
                {
                    // std::cout << this->get_id() << "::tick()\n";
                    if (!this->external_resource->telem->health_all_ok())
                    {
                        // std::cout << "Waiting for system to be ready\n";
                        // std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                    else 
                    {
                        this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS);
                        return;
                    }
                }


                void handle_stop() override
                {
                    // auto res = this->external_resource->action->hold();
                    // while (res != mavsdk::Action::Result::Success)
                    // {
                    //     res = this->external_resource->action->hold();
                    // }
                }
            };


            class Arm : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                public:
                Arm(std::shared_ptr<uavsdk::agrilab::CmdExternalResources> ext_res, std::shared_ptr<uavsdk::agrilab::TakeOffData> data, std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_external_resource(ext_res);
                    this->set_command_data(data);
                    this->set_id("arm");
                }


                protected:
                void logic_tick() override
                {
                    // std::cout << this->get_id() << "::tick()\n";
                    // std::cout << "System ready\n";

                    // std::cout << "Arming...\n";
                    const mavsdk::Action::Result arm_result = this->external_resource->action->arm();
                    if (arm_result != mavsdk::Action::Result::Success) 
                    {
                        std::cerr << "Arming failed: " << arm_result << '\n';
                        this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "156");
                        return;
                    }
                    else
                    {
                        this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS);
                        return;
                    }
                }

                
                void handle_stop() override
                {
                    // auto res = this->external_resource->action->hold();
                    // while (res != mavsdk::Action::Result::Success)
                    // {
                    //     res = this->external_resource->action->hold();
                    // }
                }
            };


            class StartTakeoff : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                public:
                StartTakeoff(std::shared_ptr<uavsdk::agrilab::CmdExternalResources> ext_res, std::shared_ptr<uavsdk::agrilab::TakeOffData> data, std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_external_resource(ext_res);
                    this->set_command_data(data);
                    this->set_id("start_takeoff");
                }


                protected:
                void logic_tick() override
                {
                    // std::cout << this->get_id() << "::tick()\n";
                    // float z_coor = std::dynamic_pointer_cast<uavsdk::fcu_tel_collector::PositionData>(this->external_resource->telem->get_msg()->at("uav_position"))->get_msg().relative_altitude_m;

                    // this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>("z_coor")->set_data(z_coor);

                    this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>("z_coor")->set_data(this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::data_adapters::ros2::geometry_msgs::Pose>>("mission_item")->get_data().position.z);
                    // z_coor = mission_item.position.z;
                    mavsdk::Action::Result takeoff_result = this->external_resource->action->set_takeoff_altitude(this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>("z_coor")->get_data());
                    takeoff_result = this->external_resource->action->takeoff();

                    if (takeoff_result != mavsdk::Action::Result::Success) 
                    {
                        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
                        this->stop(uavsdk::command_manager::ExecutionResult::FAILED, "175");
                        return;
                    }
                    else
                    {
                        this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>("target_alt")->set_data(this->external_resource->action->get_takeoff_altitude().second);
                        this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>("current_height")->set_data(0);
                        this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS);
                        return;
                    }
                }

                
                void handle_stop() override
                {
                    // auto res = this->external_resource->action->hold();
                    // while (res != mavsdk::Action::Result::Success)
                    // {
                    //     res = this->external_resource->action->hold();
                    // }
                }
            };


            class CheckPosition : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                public:
                CheckPosition(std::shared_ptr<uavsdk::agrilab::CmdExternalResources> ext_res, std::shared_ptr<uavsdk::agrilab::TakeOffData> data, std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_external_resource(ext_res);
                    this->set_command_data(data);
                    this->set_id("check_position");
                }


                protected:
                void logic_tick() override
                {
                    // std::cout << this->get_id() << "::tick()\n";
                    #warning Будет ли дрон при take_off прям точно долетать до нужной точки по высоте?
                    
                    auto current_uav_position = std::dynamic_pointer_cast<fcu_tel_collector::PositionData>(this->external_resource->telem->get_msg()->at("uav_position"));

                    if (current_uav_position->get_msg().relative_altitude_m < this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>("target_alt")->get_data() - 1) 
                    {
                        this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>("current_height")->set_data(current_uav_position->get_msg().relative_altitude_m);
                        // current_height = telemetry->position().relative_altitude_m;
                        // std::cout << "TAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFFTAKINGOFF\n";
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));                    
                    } 
                    else
                    {
                        // states.erase(states.begin());
                        this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS, "203");
                        return;
                    }
                }

                
                void handle_stop() override
                {
                    // auto res = this->external_resource->action->hold();
                    // while (res != mavsdk::Action::Result::Success)
                    // {
                    //     res = this->external_resource->action->hold();
                    // }
                }
            };

            class TakeOff : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, uavsdk::agrilab::TakeOffData>
            {
                public:
                TakeOff(std::shared_ptr<CmdExternalResources> ext_res, std::shared_ptr<TakeOffData> data, std::shared_ptr<useful_di::UniMapStr> blackboard_init) : CommandInterfaceWithBlackboard(blackboard_init)
                {
                    this->set_external_resource(ext_res);
                    this->set_command_data(data);
                    this->set_id("take_off"); // path_following

                    auto z_coor = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>(0);
                    this->add_data_to_bb("z_coor", z_coor);

                    auto target_alt = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>(0);
                    this->add_data_to_bb("target_alt", target_alt);

                    auto current_height = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<float>>(0);
                    this->add_data_to_bb("current_height", current_height);

                    uavsdk::data_adapters::ros2::geometry_msgs::Pose pose;
                    auto mission_item = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::data_adapters::ros2::geometry_msgs::Pose>>(pose);
                    this->add_data_to_bb("mission_item", mission_item);

                    auto check_offboard = std::make_shared<CheckOffboard>(this->external_resource, this->get_bb_p());
                    auto check_setpoint = std::make_shared<CheckSetpoint>(this->external_resource, this->command_data, this->get_bb_p());
                    auto wait_for_health = std::make_shared<WaitForHealth>(this->external_resource, this->command_data, this->get_bb_p());
                    auto arm = std::make_shared<Arm>(this->external_resource, this->command_data, this->get_bb_p());
                    auto start_takeoff = std::make_shared<StartTakeoff>(this->external_resource, this->command_data, this->get_bb_p());
                    auto check_position = std::make_shared<CheckPosition>(this->external_resource, this->command_data, this->get_bb_p());

                    this->add_stage(check_offboard);
                    this->add_stage(check_setpoint);
                    this->add_stage(wait_for_health);
                    this->add_stage(arm);
                    this->add_stage(start_takeoff);
                    this->add_stage(check_position);
                }


                protected:
                void handle_stop() override
                {
                    auto res = this->external_resource->action->hold();
                    while (res != mavsdk::Action::Result::Success)
                    {
                        res = this->external_resource->action->hold();
                    }
                }
            };
        };
    };
};