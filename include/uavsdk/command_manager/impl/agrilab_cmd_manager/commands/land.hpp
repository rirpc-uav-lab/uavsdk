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
            //check landed state c 2767 по 2770
            class CheckLandedState : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, void>
            {
                public:
                    CheckLandedState(std::shared_ptr<CmdExternalResources> ext_res, std::shared_ptr<useful_di::UniMapStr> _blackboard, std::shared_ptr<TakeOffData> data) : CommandInterfaceWithBlackboard(_blackboard)
                    {
                        this->set_external_resource(ext_res);
                        this->set_command_data(data);
                        this->set_id("check_landed_state");
                    }

                protected:
                    void logic_tick() override
                    {
                        std::cout << this->get_id() << "::tick()\n";
                        auto landed_state = std::dynamic_pointer_cast<fcu_tel_collector::LandedStateData>(this->external_resource->telem->get_msg()->at("landed_state"));

                        if(landed_state->get_msg() == mavsdk::Telemetry::LandedState::OnGround)
                        {
                            this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS);
                            return;
                        }
                        
                    }
                    void handle_stop() override
                    {
                        
                    };
            };          
            // land 2783 2790
            class Land : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, void>
            {
                public:
                    Land(std::shared_ptr<CmdExternalResources> ext_res, std::shared_ptr<useful_di::UniMapStr> _blackboard, std::shared_ptr<TakeOffData> data) : CommandInterfaceWithBlackboard(_blackboard)
                    {
                        this->set_external_resource(ext_res);
                        this->set_command_data(data);
                        this->set_id("land");
                    }

                protected:
                    void logic_tick() override
                    {
                        std::cout << this->get_id() << "::tick()\n";
                        //  ИСПРАВИТЬ,когда Андрей освободится
                        int timeout_counter = 100;
                        
                        
                        mavsdk::Action::Result res = this->external_resource->action->land();
                        if(res != mavsdk::Action::Result::Success and timeout_counter )
                        {
                            res = this->external_resource->action->land();
                            std::this_thread::sleep_for(100ms);
                        }
                        

                        this->add_data_to_bb("land_state_res",std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<mavsdk::Action::Result>>(res));

                    }
                    void handle_stop() override
                    {
                        
                    };
            };     
            // land check 2791-2813
            class LandCheck : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string, uavsdk::agrilab::CmdExternalResources, void>
            {
                public:
                    LandCheck(std::shared_ptr<CmdExternalResources> ext_res, std::shared_ptr<useful_di::UniMapStr> _blackboard, std::shared_ptr<TakeOffData> data) : CommandInterfaceWithBlackboard(_blackboard)
                    {
                        this->set_external_resource(ext_res);
                        this->set_command_data(data);
                        this->set_id("land_check");
                    }
                protected:
                    void logic_tick() override
                    {
                        std::cout << this->get_id() << "::tick()\n";
                        mavsdk::Action::Result res = this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<mavsdk::Action::Result>>("land_state_res")->get_data();
                        if(res == mavsdk::Action::Result::Success)
                        {
                            auto flight_mode = std::dynamic_pointer_cast<fcu_tel_collector::LandedStateData>(this->external_resource->telem->get_msg()->at("landed_state"));
                            while(flight_mode->get_msg() != mavsdk::Telemetry::LandedState::OnGround)
                            {
                                auto flight_mode = std::dynamic_pointer_cast<fcu_tel_collector::FlightModeData>(this->external_resource->telem->get_msg()->at("flight_mode"));
                                if(flight_mode->get_msg() != mavsdk::Telemetry::FlightMode::Land)
                                {
                                    mavsdk::Action::Result res = this->external_resource->action->land();
                                }
                            }
                            this->stop(uavsdk::command_manager::ExecutionResult::SUCCESS);
                            return;
                        }
                        else
                        {
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED);
                            return;
                        }

                    }
                    void handle_stop() override
                    {
                        
                    };

            };
        };
    };
};

