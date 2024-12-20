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

    TelemetryData telemetry_data;
    auto observer_p = std::make_shared<AttitudeEulerObserver>(telemetry_data.attitude_euler);
    std::make_shared<AttitudeEulerCollector>(telemetry, observer_p);

    while (true)
    {
        std::cout << telemetry_data.attitude_euler->get_msg().pitch_deg << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}