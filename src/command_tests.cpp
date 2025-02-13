#include <iostream>
#include <string>

#include <mavsdk/plugins/action/action.h>

#include <uavsdk/fcu_telemetry_collector/mavsdk_fcu_telemetry_collector.hpp>
#include <uavsdk/useful_data_lib/useful_data_impl.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/cmd_manager.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/commands/take_off.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/command_data.hpp>
#include <uavsdk/command_manager/impl/agrilab_cmd_manager/external_resource.hpp>
#include <uavsdk/data_adapters/ros2/geometry_msgs/pose.hpp>


int main()
{
    using namespace uavsdk;
    using namespace mavsdk;

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection("udp://:14540");

    while (connection_result != ConnectionResult::Success) 
    {
        std::cerr << "Connection failed: " << connection_result << '\n';
        throw std::exception();
    }

    auto system = mavsdk.first_autopilot(15.0);
    std::shared_ptr<Telemetry> telemetry = std::make_shared<Telemetry>(system.value());
    std::shared_ptr<mavsdk::Action> action = std::make_shared<mavsdk::Action>(system.value());

    std::shared_ptr<fcu_tel_collector::TelemetryDataComposite> telem_data = std::make_shared<fcu_tel_collector::TelemetryDataComposite>(telemetry);

    uavsdk::agrilab::commands::Executor executor;

    auto external_resource = std::make_shared<uavsdk::agrilab::CmdExternalResources>();
    external_resource->action = action;
    external_resource->telem = telem_data;
    
    auto command_data = std::make_shared<uavsdk::agrilab::TakeOffData>();
    uavsdk::data_adapters::ros2::geometry_msgs::Pose pose;
    pose.position.z = 15;
    command_data->height_pose.push_back(pose); 

    auto take_off_cmd = std::make_shared<uavsdk::agrilab::commands::TakeOff>(external_resource, command_data);
    
    executor.set_command(take_off_cmd);

    uavsdk::command_manager::StartExecutionResult res = executor.start_execution();
    std::shared_future<uavsdk::command_manager::ExecutionResult> res_future = executor.get_result_future();

    if (res == uavsdk::command_manager::StartExecutionResult::STARTED)
    {
        auto status = res_future.wait_for(std::chrono::milliseconds(10));
        while (!(status == std::future_status::ready))
        {
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

    return 0;
}