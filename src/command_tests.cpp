#include <iostream>
#include <string>

#include <mavsdk/plugins/action/action.h>

#include <uavsdk/fcu_telemetry_collector/mavsdk_fcu_telemetry_collector.hpp>
#include <uavsdk/useful_data_lib/useful_data_impl.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/cmd_manager.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/command_data.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/external_resource.hpp>
#include <uavsdk/data_adapters/ros2/geometry_msgs/pose.hpp>

#include <uavsdk/command_manager/impl/agrilab_cmd_manager/commands/take_off.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/commands/path_following.hpp>

int main()
{
    using namespace uavsdk;
    using namespace mavsdk;
    std::cout << "Success!\n";

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection("udp://:14540");

    while (connection_result != ConnectionResult::Success) 
    {
        std::cerr << "Connection failed: " << connection_result << '\n';
        throw std::exception();
    }

    auto system = mavsdk.first_autopilot(15.0);
    std::shared_ptr<Telemetry> telemetry = std::make_shared<Telemetry>(system.value());
    std::shared_ptr<mavsdk::Offboard> offboard = std::make_shared<mavsdk::Offboard>(system.value());
    std::shared_ptr<mavsdk::Action> action = std::make_shared<mavsdk::Action>(system.value());

    std::shared_ptr<fcu_tel_collector::TelemetryDataComposite> telem_data = std::make_shared<fcu_tel_collector::TelemetryDataComposite>(telemetry);

    uavsdk::agrilab::commands::Executor executor;
    std::cout << "Success!\n";


    auto pid = std::make_shared<position_reg::PID>();
    auto reg = std::make_shared<position_reg::PositionReg>();
    auto external_resource = std::make_shared<uavsdk::agrilab::CmdExternalResources>();
    external_resource->action = action;
    external_resource->telem = telem_data;
    external_resource->offboard = offboard;
    external_resource->pid = pid;
    external_resource->reg = reg;
    
    auto command_data = std::make_shared<uavsdk::agrilab::TakeOffData>();
    uavsdk::data_adapters::ros2::geometry_msgs::Pose pose;
    pose.position.z = 15;
    command_data->height_pose.push_back(pose); 

    std::shared_ptr<useful_di::UniMapStr> blackboard_init =  std::make_shared<useful_di::UniMapStr>();

    auto speed_limit = std::make_shared<uavsdk::data_adapters::cxx::BasicDataAdapter<double>>(5.0);
    blackboard_init->add_data("speed_limit", speed_limit);

    auto take_off_cmd = std::make_shared<uavsdk::agrilab::commands::TakeOff>(external_resource, command_data, blackboard_init);
    
    executor.set_command(take_off_cmd);

    uavsdk::command_manager::StartExecutionResult res = executor.start_execution();
    std::shared_future<uavsdk::command_manager::ExecutionResult> res_future = executor.get_result_future();

    std::cout << "Success!\n";
    if (res == uavsdk::command_manager::StartExecutionResult::STARTED)
    {
        auto status = res_future.wait_for(std::chrono::milliseconds(10));
        while (!(status == std::future_status::ready))
        {
            std::cout << "B_flag\n";
            status = res_future.wait_for(std::chrono::milliseconds(10));
        // std::cout << "Waiting.\n";
            // std::cout << "Future is valid " << res_future.valid() << "\n";
            // if (status == std::future_status::deferred)
            // {
            //     std::cout << "Future status: " << "deferred" << "\n\n";
            // }
            // else if (status == std::future_status::ready)
            // {
            //     std::cout << "Future status: " << "ready" << "\n\n";
            // }
            // else if (status == std::future_status::timeout)
            // {
            //     std::cout << "Future status: " << "timeout" << "\n\n";
            // }
        }
        auto exres = res_future.get();
        if (exres == uavsdk::command_manager::ExecutionResult::SUCCESS)
        {
            std::cout << "Success!\n";
        }
        else
        {
            std::cout << "Failed!\n";
        }
        status = res_future.wait_for(std::chrono::milliseconds(10));
    }

    executor.stop_execution();

    auto path_following_data = std::make_shared<uavsdk::agrilab::PathFollowingData>();
    
    pose = uavsdk::data_adapters::ros2::geometry_msgs::Pose();
    pose.position.x = 47.398170327054473;
    pose.position.y = 8.5456490218639658;
    pose.position.z = 20.0;
    path_following_data->global_trajectory_wgs84.push_back(pose);


    pose = uavsdk::data_adapters::ros2::geometry_msgs::Pose();
    pose.position.x = 47.398241338125118;
    pose.position.y = 8.5455360114574432;
    pose.position.z = 20.0;
    path_following_data->global_trajectory_wgs84.push_back(pose);

    pose = uavsdk::data_adapters::ros2::geometry_msgs::Pose();
    pose.position.x = 47.398139363821485;
    pose.position.y = 8.5453846156597137;
    pose.position.z = 20.0;
    path_following_data->global_trajectory_wgs84.push_back(pose);

    pose = uavsdk::data_adapters::ros2::geometry_msgs::Pose();
    pose.position.x = 47.398058617228855;
    pose.position.y = 8.5454618036746979;
    pose.position.z = 20.0;
    path_following_data->global_trajectory_wgs84.push_back(pose);


    pose = uavsdk::data_adapters::ros2::geometry_msgs::Pose();
    pose.position.x = 47.398170327054473;
    pose.position.y = 8.5456490218639658;
    pose.position.z = 20.0;
    path_following_data->global_trajectory_wgs84.push_back(pose);


    auto path_following = std::make_shared<uavsdk::agrilab::commands::PathFollowing>(external_resource, path_following_data, blackboard_init);
    
    executor.set_command(path_following);


    uavsdk::command_manager::StartExecutionResult res_pf = executor.start_execution();
    std::shared_future<uavsdk::command_manager::ExecutionResult> res_future_pf = executor.get_result_future();

    if (res_pf == uavsdk::command_manager::StartExecutionResult::STARTED)
    {
        auto status = res_future_pf.wait_for(std::chrono::milliseconds(10));
        while (!(status == std::future_status::ready))
        {
            status = res_future_pf.wait_for(std::chrono::milliseconds(10));
            // std::cout << "Waiting.\n";
            // std::cout << "Future is valid " << res_future_pf.valid() << "\n";
            // if (status == std::future_status::deferred)
            // {
            //     std::cout << "Future status: " << "deferred" << "\n\n";
            // }
            // else if (status == std::future_status::ready)
            // {
            //     std::cout << "Future status: " << "ready" << "\n\n";
            // }
            // else if (status == std::future_status::timeout)
            // {
            //     std::cout << "Future status: " << "timeout" << "\n\n";
            // }
        }
        auto exres = res_future_pf.get();
        if (exres == uavsdk::command_manager::ExecutionResult::SUCCESS)
        {
            std::cout << "Success!\n";
        }
        else
        {
            std::cout << "Failed!\n";
        }
        status = res_future_pf.wait_for(std::chrono::milliseconds(10));
    }

    return 0;
}