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
            class GoToLocation : public uavsdk::command_manager::CommandInterface<std::string,uavsdk::agrilab::CmdExternalResources,uavsdk::agrilab::GoToLocationData>
            {
                public:
                    GoToLocation(std::shared_ptr<CmdExternalResources> ext_res, std::shared_ptr<GoToLocationData> data)
                    {
                        this->set_external_resource(ext_res);
                        this->set_command_data(data);
                        this->set_id("go_to_location");

                        states = {"go_to_location","hold","distance_wgs","distance_wgs_hold"};
                    }
                protected:
                    double distance_wgs84(double lat1, double lon1, double lat2, double lon2)
                    {
                                const int R = 6371e3;

                                const double phi1 = lat1 * M_PI / 180.0; 
                                const double phi2 = lat2 * M_PI / 180.0; 

                                const double dphi = phi2 - phi1;
                                const double dlambda = (lon2 - lon1) * M_PI / 180.0;

                                const double a = sin(dphi/2) * sin(dphi/2) + cos(phi1) * cos(phi2) * sin(dlambda/2) * sin(dlambda/2);
                                const double c = 2 * atan2(sqrt(a), sqrt(1-a));

                                return R * c; // meters
                    }

                    void logic_tick() override
                    {
                        if(states.at(0) == "go_to_location")
                        {
                            #warning HOLD COMMAND ON AVOID REACCEPTANCE IS OVERRIDING GO TO
                            mavsdk::Action::Result res = this->external_resource->action->goto_location(
                                this->command_data->global_location_wgs84.at(0).pose.x,
                                this->command_data->global_location_wgs84.at(0).pose.y,
                                this->command_data->global_location_wgs84.at(0).pose.z + std::dynamic_pointer_cast<fcu_tel_collector::PositionData>(this->external_resource->telem->get_msg()->at("uav_position"))->get_msg().absolute_altitude_m,
                                0.0f);
                            
                        }
                        else if (states.at(0) == "hold")
                        {
                            auto res = this->external_resource->action->hold();
                            while(res != mavsdk::Action::Result::Success)
                            {
                                std::this_thread::sleep_for(50ms);
                                res = this->external_resource->action->hold();
                            }
                            this->stop(uavsdk::command_manager::ExecutionResult::FAILED);

                        }
                        else if(states.at(0) == "distance_wgs")
                        {
                            double dr = distance_wgs84(std::dynamic_pointer_cast<fcu_tel_collector::PositionData>(this->external_resource->telem->get_msg()->at("uav_position"))->get_msg().latitude_deg,
                            std::dynamic_pointer_cast<fcu_tel_collector::PositionData>(this->external_resource->telem->get_msg()->at("uav_position"))->get_msg().longitude_deg,
                            this->command_data->global_location_wgs84.at(0).pose.x,
                            this->command_data->global_location_wgs84.at(0).pose.y
                            );

                        }
                        else if(states.at(0) == "distance_wgs_hold")
                        {
                            
                        }
        
                    }


                    void handle_stop() override
                    {

                    }
                private:
                std::vector<std::string> states;
            };
        }
    }
}
