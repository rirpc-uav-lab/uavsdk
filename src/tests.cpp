#include <uavsdk/fcu_telemetry_collector/mavsdk_fcu_telemetry_collector.hpp>
#include <iostream>

int main()
{
    using namespace fcu_tel_collector;
    using namespace mavsdk;

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::CompanionComputer}};
    ConnectionResult connection_result = mavsdk.add_any_connection("udp://:14540");

    while (connection_result != ConnectionResult::Success) 
    {
        std::cerr << "Connection failed: " << connection_result << '\n';
        throw std::exception();
    }

    auto system = mavsdk.first_autopilot(15.0);
    std::shared_ptr<Telemetry> telemetry = std::make_shared<Telemetry>(system.value());


    TelemetryData telem_data(telemetry);

    while (true)
    {
        std::cout << "attitude_euler: " << telem_data.get_attitude_euler_data()->get_data() << "\n";
        std::cout << "uav_position: " << telem_data.get_uav_position_data()->get_data() << "\n";
        std::cout << "uav_position: " << telem_data.get_flight_mode_data()->get_data() << "\n\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}