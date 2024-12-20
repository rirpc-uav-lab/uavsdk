#pragma once

#include <thread>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <uavsdk/uav_control_interfaces/data_collector_interfaces.hpp>
#include <uavsdk/utils/coversion_functions.hpp>



namespace fcu_tel_collector /// fcu_telemetry_collector_mavsdk
{
    // ############################################### 
    // ###############  Data Classes   ############### 
    // ############################################### 

    class DistanceSensorData : public uav_ci::data_ci::TypedDataObject<mavsdk::Telemetry::DistanceSensor>
    {
        public:
        DistanceSensorData(mavsdk::Telemetry::DistanceSensor msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::DistanceSensor>("mavsdk::Telemetry::DistanceSensor", message_name)
        {
            this->msg = msg;
            
            this->data = nlohmann::json();
            this->data["current_distance_m"] = msg.current_distance_m;
            this->data["maximum_distance_m"] = msg.maximum_distance_m;
            this->data["minimum_distance_m"] = msg.minimum_distance_m;
            this->data["orientation.pitch_deg"] = msg.orientation.pitch_deg;
            this->data["orientation.roll_deg"] = msg.orientation.roll_deg;
            this->data["orientation.yaw_deg"] = msg.orientation.yaw_deg;
        }
    };


    class FlightModeData : public uav_ci::data_ci::TypedDataObject<mavsdk::Telemetry::FlightMode>
    {
        public:
        FlightModeData(mavsdk::Telemetry::FlightMode msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::FlightMode>("mavsdk::Telemetry::FlightMode", message_name)
        {
            this->msg = msg;
            
            data = nlohmann::json();
            data["flight_mode"] = utils::conversions::convert_flight_mode_to_str(msg);
        }
    };


    class AttitudeEulerData : public uav_ci::data_ci::TypedDataObject<mavsdk::Telemetry::EulerAngle>
    {
        public:
        AttitudeEulerData(mavsdk::Telemetry::EulerAngle msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::EulerAngle>("mavsdk::Telemetry::EulerAngle", message_name)
        {
            this->msg = msg;
            
            data = nlohmann::json();
            data["pitch_deg"] = msg.pitch_deg;
            data["roll_deg"] = msg.roll_deg;
            data["yaw_deg"] = msg.yaw_deg;
        }
    };


    class PositionData : public uav_ci::data_ci::TypedDataObject<mavsdk::Telemetry::Position>
    {
        public:
        PositionData(mavsdk::Telemetry::Position msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::Position>("mavsdk::Telemetry::Position", message_name)
        {
            this->msg = msg;
            
            data = nlohmann::json();
            data["absolute_altitude_m"] = msg.absolute_altitude_m;
            data["latitude_deg"] = msg.latitude_deg;
            data["longitude_deg"] = msg.longitude_deg;
            data["relative_altitude_m"] = msg.relative_altitude_m;
        }
    };


    class HeadingData : public uav_ci::data_ci::TypedDataObject<mavsdk::Telemetry::Heading>
    {
        public:
        HeadingData(mavsdk::Telemetry::Heading msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::Heading>("mavsdk::Telemetry::Heading", message_name)
        {
            this->msg = msg;
            
            data = nlohmann::json();
            data["heading_deg"] = msg.heading_deg;
        }
    };







    // ############################################### 
    // ##########  Data Collector Classes   ########## 
    // ############################################### 
    class DistanceSensorCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::DistanceSensor, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::DistanceSensorHandle handle;
    public:
        DistanceSensorCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::DistanceSensor, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_distance_sensor([this](mavsdk::Telemetry::DistanceSensor data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<DistanceSensorData>(data, "distance_sensor"));
                }).detach();
            });


        }

        ~DistanceSensorCollector()
        {
            telemetry->unsubscribe_distance_sensor(handle);
        }
    };


    class FlightModeCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::FlightMode, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::FlightModeHandle handle;
    public:
        FlightModeCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::FlightMode, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_flight_mode([this](mavsdk::Telemetry::FlightMode data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<FlightModeData>(data, "flight_mode"));
                }).detach();
            });
        }

        ~FlightModeCollector()
        {
            telemetry->unsubscribe_flight_mode(handle);
        }
    };


    class AttitudeEulerCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::EulerAngle, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::AttitudeEulerHandle handle;
    public:
        AttitudeEulerCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::EulerAngle, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_attitude_euler([this](mavsdk::Telemetry::EulerAngle data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<AttitudeEulerData>(data, "attitude_euler"));
                }).detach();
            });
        }

        ~AttitudeEulerCollector()
        {
            telemetry->unsubscribe_attitude_euler(handle);
        }
    };
    

    class HomePositionCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::Position, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::HomeHandle handle;
    public:
        HomePositionCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::Position, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_home([this](mavsdk::Telemetry::Position data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<PositionData>(data, "home_position"));
                }).detach();
            });
        }


        ~HomePositionCollector()
        {
            telemetry->unsubscribe_home(handle);
        }
    };


    class UavPositionCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::Position, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::HomeHandle handle;
    public:
        UavPositionCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::Position, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_position([this](mavsdk::Telemetry::Position data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<PositionData>(data, "uav_position"));
                }).detach();
            });
        }


        ~UavPositionCollector()
        {
            telemetry->unsubscribe_position(handle);
        }
    };


    class HeadingCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::Heading, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::HeadingHandle handle;
    public:
        HeadingCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::Heading, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_heading([this](mavsdk::Telemetry::Heading data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<HeadingData>(data, "uav_heading"));
                }).detach();
            });
        }


        ~HeadingCollector()
        {
            telemetry->unsubscribe_heading(handle);
        }
    };

















    // ############################################### 
    // ###########  Data Observer Classes   ########## 
    // ############################################### 
    class DistanceSensorObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::DistanceSensor, nlohmann::json>
    {
    public:
        DistanceSensorObserver(std::shared_ptr<DistanceSensorData> data) : uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::DistanceSensor, nlohmann::json>(data) {}
    };


    class FlightModeObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::FlightMode, nlohmann::json>
    {
    public:
        FlightModeObserver(std::shared_ptr<FlightModeData> data) :  uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::FlightMode, nlohmann::json>(data) {}
    };


    class AttitudeEulerObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::EulerAngle, nlohmann::json>
    {
    public:
        AttitudeEulerObserver(std::shared_ptr<AttitudeEulerData> data) : uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::EulerAngle, nlohmann::json>(data) {}
    };


    class HomePositionObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Position, nlohmann::json>
    {
    public:
        HomePositionObserver(std::shared_ptr<PositionData> data) : uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Position, nlohmann::json>(data) {}
    };














    // ############################################### 
    // ##############  Client Classes   ############## 
    // ############################################### 
    struct TelemetryData
    {
        public:
        TelemetryData()
        {
            distance_sensor = std::make_shared<DistanceSensorData>(mavsdk::Telemetry::DistanceSensor(), "distance_sensor");
            flight_mode = std::make_shared<FlightModeData>(mavsdk::Telemetry::FlightMode(), "flight_mode");
            attitude_euler = std::make_shared<AttitudeEulerData>(mavsdk::Telemetry::EulerAngle(), "attitude_euler");
            home_position = std::make_shared<PositionData>(mavsdk::Telemetry::Position(), "home_position");
            uav_position = std::make_shared<PositionData>(mavsdk::Telemetry::Position(), "uav_position");
            uav_heading = std::make_shared<HeadingData>(mavsdk::Telemetry::Heading(), "uav_heading");
        }
        std::shared_ptr<DistanceSensorData> distance_sensor;
        std::shared_ptr<FlightModeData> flight_mode;
        std::shared_ptr<AttitudeEulerData> attitude_euler;
        std::shared_ptr<PositionData> home_position;
        std::shared_ptr<PositionData> uav_position;
        std::shared_ptr<HeadingData> uav_heading;
    };


    // class FCUTelemetryCollector
    // {
    // public:
    //     FCUTelemetryCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p) : telemetry(telemetry_p) 
    //     {
    //         subscribe_distance_sensor();
    //         subscribe_flight_mode();
    //         subscribe_attitude_euler();
    //         subscribe_home();
    //         subscribe_position();
    //         subscribe_heading();
    //         subscribe_health();
    //         subscribe_gps_info();
    //         subscribe_battery();
    //     }

    // private:
    //     std::shared_ptr<mavsdk::Telemetry> telemetry;
    //     TelemetryData telemetry_data;

    //     inline std::shared_ptr<DistanceSensorData> subscribe_distance_sensor()
    //     {
    //         auto observer_p = std::make_shared<DistanceSensorObserver>(telemetry_data.distance_sensor);
    //         std::make_shared<DistanceSensorCollector>(telemetry, telemetry_data.distance_sensor);
    //     }

    //     inline void subscribe_flight_mode()
    //     {
    //         auto observer_p = std::make_shared<FlightModeObserver>(telemetry_data.flight_mode);
    //         std::make_shared<FlightModeCollector>(telemetry, telemetry_data.flight_mode);
    //     }

    //     inline void subscribe_attitude_euler()
    //     {
    //         auto observer_p = std::make_shared<AttitudeEulerObserver>(telemetry_data.attitude_euler);
    //         std::make_shared<AttitudeEulerCollector>(telemetry, telemetry_data.attitude_euler);
    //     }

    //     inline void subscribe_home()
    //     {
    //         auto observer_p = std::make_shared<HomePositionObserver>(telemetry_data.home_position);
    //         std::make_shared<HomePositionCollector>(telemetry, telemetry_data.home_position);
    //     }

    //     inline void subscribe_position()
    //     {
    //         auto observer_p = std::make_shared<HomePositionObserver>(telemetry_data.uav_position);
    //         std::make_shared<UavPositionCollector>(telemetry, telemetry_data.uav_position);
    //     }
        
    //     inline void subscribe_heading();

    //     inline void subscribe_health();

    //     inline void subscribe_gps_info();

    //     inline void subscribe_battery();
    // };
};
