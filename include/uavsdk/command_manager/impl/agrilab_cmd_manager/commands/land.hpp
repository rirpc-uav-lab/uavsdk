#pragma once

#include <string>
#include <chrono>
#include <thread>

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
            class LandStart : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
            {
                public:
                LandStart(std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_id("land_start");
                    this->___set_type();
                }

                protected:
                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    std::cout << this->get_id() << "::tick()\n";
                    auto landed_state = std::dynamic_pointer_cast<fcu_tel_collector::LandedStateData>(this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->telem->get_msg()->at("landed_state"));

                    if(landed_state->get_msg() == mavsdk::Telemetry::LandedState::OnGround)
                    {
                        this->stop();
                        return uavsdk::command_manager::ExecutionResult::SUCCESS;
                    }
                    
                    
                    mavsdk::Action::Result res = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->action->land();
                    if(res != mavsdk::Action::Result::Success)
                    {
                        res = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->action->land();
                        return uavsdk::command_manager::ExecutionResult::RUNNING;
                    }
                    

                    this->add_data_to_bb("land_state_res",std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<mavsdk::Action::Result>>(res));
                    return uavsdk::command_manager::ExecutionResult::SUCCESS;
                }
                void handle_stop() override
                {
                    
                };
            };     


            // land check 2791-2813
            class LandCheck : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
            {
                public:
                LandCheck(std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_id("land_check");
                    this->___set_type();
                }
                protected:
                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    std::cout << this->get_id() << "::tick()\n";
                    mavsdk::Action::Result res = this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<mavsdk::Action::Result>>("land_state_res")->get_data();
                    if(res == mavsdk::Action::Result::Success)
                    {
                        auto landed_state = std::dynamic_pointer_cast<fcu_tel_collector::LandedStateData>(this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->telem->get_msg()->at("landed_state"));
                        if(landed_state->get_msg() != mavsdk::Telemetry::LandedState::OnGround)
                        {
                            auto flight_mode = std::dynamic_pointer_cast<fcu_tel_collector::FlightModeData>(this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->telem->get_msg()->at("flight_mode"));
                            if(flight_mode->get_msg() != mavsdk::Telemetry::FlightMode::Land)
                            {
                                mavsdk::Action::Result res = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->action->land();
                                return uavsdk::command_manager::ExecutionResult::RUNNING;
                            }
                            return uavsdk::command_manager::ExecutionResult::RUNNING;
                        }
                        else
                        {
                            this->stop();
                            return uavsdk::command_manager::ExecutionResult::SUCCESS;
                        }
                    }
                    else
                    {
                        this->stop();
                        return uavsdk::command_manager::ExecutionResult::FAILED;
                    }

                }


                void handle_stop() override
                {
                    
                };
            };


            class Land : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
            {
                public:
                Land(std::shared_ptr<useful_di::UniMapStr> _blackboard) : CommandInterfaceWithBlackboard(_blackboard)
                {
                    this->set_id("land");
                    this->___set_type();


                    auto land_start = std::make_shared<LandStart>(this->get_bb_p());
                    auto land_check = std::make_shared<LandCheck>(this->get_bb_p());

                    this->add_stage(land_start);
                    this->add_stage(land_check);
                }

                protected:

                void handle_stop() override
                {
                    
                };
            };     
        };
    };
};

