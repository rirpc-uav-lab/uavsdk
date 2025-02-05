#include <uavsdk/fcu_telemetry_collector/mavsdk_fcu_telemetry_collector.hpp>
#include <iostream>


bool test_multitype_data_composite()
{
    bool success_flag = false;
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
    int i = 0;

    while (i < 30)
    {
        // std::cout << telem_data.get_data() << "\n";

        auto registry = telem_data.get_msg();
        auto gps_info = std::dynamic_pointer_cast<fcu_tel_collector::GpsInfoData>(registry->at(fcu_tel_collector::TelemetryDataTypeId::GpsInfoData));
        
        if (gps_info)
        {
            std::cout << gps_info->get_msg().num_satellites <<  std::endl;
            if (gps_info->get_msg().num_satellites > 0) success_flag = true;
        }
        else
        {
            std::cout << "Error! gps_info pointer is invalid.\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i++;
    }

    return success_flag;
}


bool test_unimap_container()
{
    bool success_flag = false;
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
    int i = 0;

    useful_di::UniMapStr data_container;

    while (i < 30)
    {
        // std::cout << telem_data.get_data() << "\n";

        auto registry = telem_data.get_msg();
        auto gps_info = std::dynamic_pointer_cast<fcu_tel_collector::GpsInfoData>(registry->at(fcu_tel_collector::TelemetryDataTypeId::GpsInfoData));
        auto battery_data = std::dynamic_pointer_cast<fcu_tel_collector::BatteryData>(registry->at(fcu_tel_collector::TelemetryDataTypeId::BatteryData));

        if (gps_info)
        {
            std::cout << gps_info->get_msg().num_satellites <<  std::endl;
        }
        else
        {
            std::cout << "Error! gps_info pointer is invalid.\n";
        }

        data_container.add_data("gps_info", gps_info);
        data_container.add_data(battery_data);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i++;
    }

    auto keys = data_container.get_present_keys();

    for (const auto& key : keys)
    {
        std::cout << key << "\n";
    }

    std::cout << "Size before removal: " << data_container.size() << "\n";
    data_container.remove_data(keys.at(0));
    std::cout << "Size after removal: " << data_container.size() << "\n";

    success_flag = true;

    return success_flag;
}


bool test_data_composite_json_map()
{
    bool success_flag = false;
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
    int i = 0;

    // useful_di::UniMapStr data_container;
    useful_di::DataCompositeJsonMap data_container;

    while (i < 30)
    {
        // std::cout << telem_data.get_data() << "\n";

        auto registry = telem_data.get_msg();
        auto gps_info = std::dynamic_pointer_cast<fcu_tel_collector::GpsInfoData>(registry->at(fcu_tel_collector::TelemetryDataTypeId::GpsInfoData));
        auto battery_data = std::dynamic_pointer_cast<fcu_tel_collector::BatteryData>(registry->at(fcu_tel_collector::TelemetryDataTypeId::BatteryData));
        auto health_data = std::dynamic_pointer_cast<fcu_tel_collector::FcuHealthData>(registry->at(fcu_tel_collector::TelemetryDataTypeId::FcuHealthData));

        data_container.add_data("gps_info", gps_info);
        data_container.add_data(battery_data);
        data_container.add_data(health_data);

        // std::cout << data_container.get_data() << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i++;
    }

    nlohmann::json data = data_container.get_data();

    std::vector<std::string> keys = data_container.get_storage_keys();

    success_flag = true;
    
    if (keys.size() < 5) success_flag = false;

    unsigned int init_counter = 0;

    for (const auto& key : keys)
    {
        if (data.at(key).at("initialized") == true) init_counter++;
    }

    if (init_counter < 10) success_flag = false;

    std::cout << "Results: \n\tkeys.size(): " << keys.size() << "\n\tinit_counter: " << init_counter << "\n";

    return success_flag;
}



int main()
{
    int i = 0;
    std::cout << "Res " << ++i << " = " << test_multitype_data_composite() << "\n";
    std::cout << "Res " << ++i << " = " << test_unimap_container() << "\n";
    std::cout << "Res " << ++i << " = " << test_data_composite_json_map() << "\n";
    std::cout << "AAA\n"; 

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

