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


    TelemetryDataComposite telem_data(telemetry);

    while (true)
    {
        // std::cout << telem_data.get_data() << "\n";

        auto registry = telem_data.get_msg();
        auto gps_info = std::dynamic_pointer_cast<fcu_tel_collector::GpsInfoData>(registry->at(fcu_tel_collector::TelemetryDataTypeId::GpsInfoData));
        
        if (gps_info)
        {
            std::cout << gps_info->get_msg().num_satellites <<  std::endl;
        }
        else
        {
            std::cout << "Error! gps_info pointer is invalid.\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

// #include <variant>
// #include <string>
// #include <iostream>

// int main()
// {
//     std::variant<int, std::string> v = 1;

//     const size_t i = 0;
//     const size_t a = i + 1;
//     std::cout << std::get<a>(v) << "\n";
// }

