#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <vector>

#include <uavsdk/command_manager/command_interface.hpp>
#include "mavsdk/plugins/action/action.h"
#include "mavsdk/plugins/offboard/offboard.h"

#include <uavsdk/command_manager/impl/agrilab_cmd_manager/external_resource.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/command_data.hpp>

#include <uavsdk/data_adapters/cxx/cxx.hpp>

#include <uavsdk/navigation/coordinates.hpp>
#include <uavsdk/navigation/conversions.hpp>

#include <position_reg/positionReg.hpp>

using namespace std::chrono_literals;

namespace uavsdk
{
    namespace agrilab
    {
        namespace commands
        {

            class CheckIncomingMisson : public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
            {
                public:
                CheckIncomingMisson (std::shared_ptr<useful_di::UniMapStr> bb_init) : CommandInterfaceWithBlackboard(bb_init)
                {
                    this->set_id("check_incoming_mission"); // path_following
                    this->___set_type();
                }


                protected:
                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    if (this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.empty())
                    {
                        this->stop();
                        return uavsdk::command_manager::ExecutionResult::RUNNING;
                    }

                    double _x = 0;
                    double _y = 0;
                    double _z = 0;

                    for (int i = 0; i < this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.size(); i++)
                    {
                        _x += this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.at(i).position.x;
                        _y += this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.at(i).position.y;
                        _z += this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.at(i).position.z;
                    }

                    _x = _x / this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.size();
                    _y = _y / this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.size();
                    _z = _z / this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.size();

                    uavsdk::navigation::coordinates::WGS84Coords p;
                    
                    p.latitude = _x;
                    p.longitude = _y;
                    p.altitude = _z;
                    this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::navigation::coordinates::WGS84Coords>>("startPoint")->set_data(p);

                    this->stop();
                    return uavsdk::command_manager::ExecutionResult::SUCCESS;
                }
            };


            class CheckStartMission : public uavsdk::command_manager::IStoppable, public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
            {
                public:
                CheckStartMission (std::shared_ptr<useful_di::UniMapStr> bb_init) : CommandInterfaceWithBlackboard(bb_init)
                {
                    this->set_id("check_start_mission"); // path_following
                    this->___set_type();

                    this->speed_limit = this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<double>>("speed_limit")->get_data();

                    this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->offboard->set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});
                    this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->offboard->start();
                }


                protected:
                uavsdk::command_manager::ExecutionResult logic_tick() override
                {
                    using namespace uavsdk::navigation::conversions;
                    using namespace uavsdk::navigation::coordinates;

                    // Получем текущую позицию дрона

                    mavsdk::Telemetry::Position position = std::dynamic_pointer_cast<uavsdk::fcu_tel_collector::PositionData>(std::dynamic_pointer_cast<useful_di::UniMapStr>(this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->telem->get_msg())->at("uav_position"))->get_msg();

                    wgs84DroneCoords.altitude = (double)position.relative_altitude_m; // высота
                    wgs84DroneCoords.longitude = (double)position.longitude_deg;       // долгота
                    wgs84DroneCoords.latitude = (double)position.latitude_deg;       // широта

                    ecefDroneCoords = wgs84ToEcef(wgs84DroneCoords);
                    enuDroneCoords = ecefToEnu(ecefDroneCoords, this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::navigation::coordinates::WGS84Coords>>("startPoint")->get_data());

                    // global_drone_pose.at(0) = enuDroneCoords.xEast;
                    // global_drone_pose.at(1) = enuDroneCoords.yNorth;
                    // global_drone_pose.at(2) = enuDroneCoords.zUp;
                    
                    global_drone_pose.xEast = enuDroneCoords.xEast;
                    global_drone_pose.yNorth = enuDroneCoords.yNorth;
                    global_drone_pose.zUp = enuDroneCoords.zUp;
                    

                    if(point_addres == adr && adr != this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.size())
                    {
                        wgs84PointCoords.latitude = this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.at(point_addres).position.x;
                        wgs84PointCoords.longitude = this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.at(point_addres).position.y;
                        wgs84PointCoords.altitude = this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.at(point_addres).position.z;

                        ecefPointCoords = wgs84ToEcef(wgs84PointCoords);
                        enuPointCoords = ecefToEnu(ecefPointCoords, this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::navigation::coordinates::WGS84Coords>>("startPoint")->get_data());

                        goalPoints.xEast = enuPointCoords.xEast;
                        goalPoints.yNorth = enuPointCoords.yNorth;
                        goalPoints.zUp = enuPointCoords.zUp;

                        adr++;
                    }       

                    cartesianDroneCoords = global_drone_pose;
                    cartesianGoalPointCoords = goalPoints;

                    x = (cartesianGoalPointCoords.xEast - cartesianDroneCoords.xEast)*(cartesianGoalPointCoords.xEast - cartesianDroneCoords.xEast);
                    y = (cartesianGoalPointCoords.yNorth - cartesianDroneCoords.yNorth)*(cartesianGoalPointCoords.yNorth - cartesianDroneCoords.yNorth);
                    z = (cartesianGoalPointCoords.zUp - cartesianDroneCoords.zUp)*(cartesianGoalPointCoords.zUp - cartesianDroneCoords.zUp);

                    distance_to_goal_point = sqrt(x+y+z);

                    if(distance_to_goal_point < r)
                    {
                        point_addres++;

                        // RCLCPP_WARN_STREAM(this->get_logger(),"THE GOAL POINT HAS BEEN UPDATED. CURRENT GOAL POINT IS: " << point_addres);
                    }                   
            
                    speeds.xEast = cartesianGoalPointCoords.xEast - cartesianDroneCoords.xEast;
                    speeds.yNorth = (cartesianGoalPointCoords.yNorth - cartesianDroneCoords.yNorth);
                    speeds.zUp = (-(cartesianGoalPointCoords.zUp - cartesianDroneCoords.zUp));

                    speed_vec_lenght = sqrt(pow(speeds.xEast,2)+pow(speeds.yNorth,2)+pow(speeds.zUp,2));

                    direction_cos = {0, 0, 0};

                    direction_cos.at(0) = speeds.xEast/speed_vec_lenght;
                    direction_cos.at(1) = speeds.yNorth/speed_vec_lenght;
                    direction_cos.at(2) = speeds.zUp/speed_vec_lenght;

                    
                    double pid_offboardSpeed = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->pid->pid(speed_vec_lenght,kp,ki,kd, 0.0001);
                    // double pid_offboardSpeed = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->pid->pid(speed_vec_lenght,kp,ki,kd, this->bb_at<uavsdk::data_adapters::cxx::BasicDataAdapter<double>>("dt")->get_data());

                    if (pid_offboardSpeed < speed_limit)
                    {
                        offboardSpeeds.xEast = direction_cos.at(0) * (pid_offboardSpeed);
                        offboardSpeeds.yNorth = direction_cos.at(1) * (pid_offboardSpeed);
                        offboardSpeeds.zUp = direction_cos.at(2) * (pid_offboardSpeed);
                    }
                    else
                    {
                        offboardSpeeds.xEast = direction_cos.at(0) * (speed_limit);
                        offboardSpeeds.yNorth = direction_cos.at(1) * (speed_limit);
                        offboardSpeeds.zUp = direction_cos.at(2) * (speed_limit);    
                    }


                    yaw = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->reg->get_yaw_deg(cartesianDroneCoords.xEast, cartesianDroneCoords.yNorth, cartesianGoalPointCoords.xEast, cartesianGoalPointCoords.yNorth) / M_PI * 180.0;

                    std::cout << "YAW: " << yaw << "\n";

                    mavsdk::Offboard::VelocityNedYaw offboard_movement{};
                    offboard_movement.east_m_s = (float)offboardSpeeds.xEast;
                    offboard_movement.north_m_s = (float)offboardSpeeds.yNorth;
                    offboard_movement.down_m_s = (float)(offboardSpeeds.zUp);
                    offboard_movement.yaw_deg = (float)yaw;


                    
                    auto current_flight_mode = std::dynamic_pointer_cast<uavsdk::fcu_tel_collector::FlightModeData>(std::dynamic_pointer_cast<useful_di::UniMapStr>(this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->telem->get_msg())->at("flight_mode"))->get_msg();
                    
                    if (current_flight_mode != mavsdk::Telemetry::FlightMode::Stabilized and current_flight_mode != mavsdk::Telemetry::FlightMode::Posctl and current_flight_mode != mavsdk::Telemetry::FlightMode::Altctl and current_flight_mode != mavsdk::Telemetry::FlightMode::Rattitude and current_flight_mode != mavsdk::Telemetry::FlightMode::Acro and current_flight_mode != mavsdk::Telemetry::FlightMode::Offboard)
                    {
                        auto res = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->offboard->start();
                    
                        if (res != mavsdk::Offboard::Result::Success)
                        {
                            return uavsdk::command_manager::ExecutionResult::RUNNING;
                        }   
                    }
                    this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->offboard->set_velocity_ned(offboard_movement);
                    

                    if(point_addres == this->bb_at<uavsdk::agrilab::PathFollowingData>("command_data")->global_trajectory_wgs84.size())
                    {
                        std::cout << "point address = " << point_addres << "\n";
                        this->stop();
                        return uavsdk::command_manager::ExecutionResult::SUCCESS;
                    }

                    return uavsdk::command_manager::ExecutionResult::RUNNING;
                }


                void handle_stop() override
                {
                    auto res = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->action->hold();
                    while (res != mavsdk::Action::Result::Success)
                    {
                        std::this_thread::sleep_for(50ms);
                        res = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->action->hold();
                    }

                    // mavsdk::Offboard::Result offboard_res = offboard->stop();

                    // while (offboard_res != mavsdk::Offboard::Result::Success) 
                    // {
                    //     RCLCPP_ERROR_STREAM(this->get_logger(), "Offboard::stop() failed: " << offboard_res << '\n');
                    //     offboard_res = offboard->stop();
                    //     std::this_thread::sleep_for(50ms);
                    // }
                }

                // std::vector <double> global_drone_pose;
                uavsdk::navigation::coordinates::ENUCoords global_drone_pose;
                uavsdk::navigation::coordinates::ENUCoords goalPoints;
                uavsdk::navigation::coordinates::ENUCoords cartesianGoalPointCoords;
                // std::vector <double> cartesianDroneCoords;
                uavsdk::navigation::coordinates::ENUCoords cartesianDroneCoords;
                // std::vector <double> startDronePose;
                std::vector <double> direction_cos;
        
                double speed_limit;

                uavsdk::navigation::coordinates::ENUCoords speeds;
                uavsdk::navigation::coordinates::ENUCoords offboardSpeeds;
                double pid_offboardSpeed;
        
                uavsdk::navigation::coordinates::ECEFCoords ecefDroneCoords;
                uavsdk::navigation::coordinates::ENUCoords enuDroneCoords;
                uavsdk::navigation::coordinates::WGS84Coords wgs84DroneCoords;
        
                uavsdk::navigation::coordinates::ECEFCoords ecefPointCoords;
                uavsdk::navigation::coordinates::ENUCoords enuPointCoords;
                uavsdk::navigation::coordinates::WGS84Coords wgs84PointCoords;        

                double speed_vec_lenght = 0.0;
                double pid_orientation_val = 0.0;
        
                double r = 0.5;
                double x,y,z;
                double distance_to_goal_point;
        
                double kp = 0.6;
                double ki = 0.0001;
                double kd = 0.0000005;
        
                double sumError = 0, devError = 0;
        
                double pitch;
                double yaw;
        
                bool get_points = false;    
        
                bool flag = true;
                bool startPositionFlag = true;
                bool arm_flag = true;
                bool takeoff_flag = true;
                bool mission_flag = false;
                bool off_flag = true;
        
                int point_addres = 0;
                int adr = 0;
            };



            class PathFollowing : public uavsdk::command_manager::IStoppable, public uavsdk::command_manager::CommandInterfaceWithBlackboard<std::string>
            {
                public:
                PathFollowing(std::shared_ptr<useful_di::UniMapStr> bb_init) : CommandInterfaceWithBlackboard(bb_init)
                {
                    this->___set_type();
                    this->set_id("take_off"); // path_following

                    std::shared_ptr<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::navigation::coordinates::WGS84Coords>> startPoint = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<uavsdk::navigation::coordinates::WGS84Coords>>(uavsdk::navigation::coordinates::WGS84Coords());
                    this->add_data_to_bb("startPoint", startPoint);

                    auto check_incoming = std::make_shared<CheckIncomingMisson>(this->get_bb_p());
                    auto mission = std::make_shared<CheckStartMission>(this->get_bb_p());

                    this->add_stage(check_incoming);
                    this->add_stage(mission);
                    // states = {"check_incoming_mission","check_upload_mission","check_subscribe_mission_progress" ,"check_start_mission","check_stop_mission"};
            }


                protected:

                void handle_stop() override
                {
                    if(states.at(0) == "check_stop_mission")
                    {
                        auto res = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->action->hold();
                        while (res != mavsdk::Action::Result::Success)
                        {
                            std::this_thread::sleep_for(50ms);
                            res = this->bb_at<uavsdk::agrilab::CmdExternalResources>("external_resources")->action->hold();
                        }

                        // result = mission->clear_mission();
                        // mission->unsubscribe_mission_progress(ms_handle);
                    }
                }


                private:
                std::vector<std::string> states; // {"check_offboard", "check_setpoint", "wait_for_health", "arm", "start_takeoff", "check_position"}

                float z_coor;
                float target_alt;
                float current_position;
                // geometry_msgs::msg::Pose mission_item;

            };
        };
    };
};

