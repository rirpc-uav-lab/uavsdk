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

    TelemetryData telem_data;
    // auto observer_p = std::make_shared<AttitudeEulerObserver>(std::make_shared<AttitudeEulerData>(mavsdk::Telemetry::EulerAngle(), "attitude_euler"));
    std::shared_ptr<AttitudeEulerCollector> attitude_euler_collector = std::make_shared<AttitudeEulerCollector>(telemetry, telem_data.get_attitude_euler_observer_pointer());
    std::shared_ptr<UavPositionCollector> uav_position_collector = std::make_shared<UavPositionCollector>(telemetry, telem_data.get_uav_position_observer_pointer());

    while (true)
    {
        if (telem_data.get_attitude_euler_observer_pointer())
        {
            std::cout << "attitude_euler: " << telem_data.get_attitude_euler_data()->get_data() << "\n\n\n";
        }
        if (telem_data.get_uav_position_observer_pointer())
        {
            std::cout << "uav_position: " << telem_data.get_uav_position_data()->get_data() << "\n\n\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}