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

        DistanceSensorData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::DistanceSensor>("mavsdk::Telemetry::DistanceSensor", message_name)
        {
            this->msg = mavsdk::Telemetry::DistanceSensor();
            
            this->data = nlohmann::json();
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
        
        FlightModeData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::FlightMode>("mavsdk::Telemetry::FlightMode", message_name)
        {
            this->msg = mavsdk::Telemetry::FlightMode();

            this->data = nlohmann::json();
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

        AttitudeEulerData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::EulerAngle>("mavsdk::Telemetry::EulerAngle", message_name)
        {
            this->msg = mavsdk::Telemetry::EulerAngle(); 
            data = nlohmann::json();
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

        PositionData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::Position>("mavsdk::Telemetry::Position", message_name)
        {
            this->msg = mavsdk::Telemetry::Position();
            
            data = nlohmann::json();
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

        HeadingData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::Heading>("mavsdk::Telemetry::Heading", message_name)
        {
            this->msg = mavsdk::Telemetry::Heading();
            
            data = nlohmann::json();
        }
    };


    class FcuHealthData : public uav_ci::data_ci::TypedDataObject<mavsdk::Telemetry::Health>
    {
        public:
        FcuHealthData(mavsdk::Telemetry::Health msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::Health>("mavsdk::Telemetry::Health", message_name)
        {
            this->msg = msg;
            
            data = nlohmann::json();
            data["is_accelerometer_calibration_ok"] = msg.is_accelerometer_calibration_ok;
            data["is_armable"] = msg.is_armable;
            data["is_global_position_ok"] = msg.is_global_position_ok;
            data["is_gyrometer_calibration_ok"] = msg.is_gyrometer_calibration_ok;
            data["is_home_position_ok"] = msg.is_home_position_ok;
            data["is_local_position_ok"] = msg.is_local_position_ok;
            data["is_magnetometer_calibration_ok"] = msg.is_magnetometer_calibration_ok;
        }

        FcuHealthData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::Health>("mavsdk::Telemetry::Health", message_name)
        {
            this->msg = mavsdk::Telemetry::Health();
            
            data = nlohmann::json();
        }
    };


    class GpsInfoData : public uav_ci::data_ci::TypedDataObject<mavsdk::Telemetry::GpsInfo>
    {
        public:
        GpsInfoData(mavsdk::Telemetry::GpsInfo msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::GpsInfo>("mavsdk::Telemetry::GpsInfo", message_name)
        {
            this->msg = msg;
            
            data = nlohmann::json();
            data["fix_type"] = utils::conversions::convert_fix_type_to_str(msg.fix_type);
            data["num_satellites"] = msg.num_satellites;
        }
        
        GpsInfoData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::GpsInfo>("mavsdk::Telemetry::GpsInfo", message_name)
        {
            this->msg = mavsdk::Telemetry::GpsInfo();
            
            data = nlohmann::json();
        }
    };


    class BatteryData : public uav_ci::data_ci::TypedDataObject<mavsdk::Telemetry::Battery>
    {
        public:
        BatteryData(mavsdk::Telemetry::Battery msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::Battery>("mavsdk::Telemetry::Battery", message_name)
        {
            this->msg = msg;
            
            data = nlohmann::json();
            data["capacity_consumed_ah"] = msg.capacity_consumed_ah;
            data["current_battery_a"] = msg.current_battery_a;
            data["id"] = msg.id;
            data["remaining_percent"] = msg.remaining_percent;
            data["temperature_degc"] = msg.temperature_degc;
            data["voltage_v"] = msg.voltage_v;
        }

        BatteryData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::Battery>("mavsdk::Telemetry::Battery", message_name)
        {
            this->msg = mavsdk::Telemetry::Battery();
            
            data = nlohmann::json();
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
        std::shared_ptr<AttitudeEulerData> data_p;
    public:
        AttitudeEulerCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::EulerAngle, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);
            handle = telemetry->subscribe_attitude_euler([this](mavsdk::Telemetry::EulerAngle data)
            {
                std::thread([this, data]()
                {
                    this->data_p = std::make_shared<AttitudeEulerData>(data, "attitude_euler");
                    this->update_data(data_p);
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


    class FcuHealthCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::Health, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::HealthHandle handle;
    public:
        FcuHealthCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::Health, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_health([this](mavsdk::Telemetry::Health data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<FcuHealthData>(data, "fcu_health"));
                }).detach();
            });
        }


        ~FcuHealthCollector()
        {
            telemetry->unsubscribe_health(handle);
        }
    };


    class GpsInfoCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::GpsInfo, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::GpsInfoHandle handle;
    public:
        GpsInfoCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, 
                        std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::GpsInfo, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_gps_info([this](mavsdk::Telemetry::GpsInfo data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<GpsInfoData>(data, "gps_info"));
                }).detach();
            });
        }


        ~GpsInfoCollector()
        {
            telemetry->unsubscribe_gps_info(handle);
        }
    };


    class BatteryCollector : public uav_ci::data_ci::SingleObserverDataCollector<mavsdk::Telemetry::Battery, nlohmann::json>
    {
    private:
        std::shared_ptr<mavsdk::Telemetry> telemetry;
        mavsdk::Telemetry::BatteryHandle handle;
    public:
        BatteryCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, 
                        std::shared_ptr<uav_ci::data_ci::DataObserverInterface<mavsdk::Telemetry::Battery, nlohmann::json>> observer)
        {
            this->telemetry = telemetry_p;
            this->attach_observer(observer);

            handle = telemetry->subscribe_battery([this](mavsdk::Telemetry::Battery data)
            {
                std::thread([this, data]()
                {
                    this->update_data(std::make_shared<BatteryData>(data, "battery_info"));
                }).detach();
            });
        }


        ~BatteryCollector()
        {
            telemetry->unsubscribe_battery(handle);
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


    class UavPositionObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Position, nlohmann::json>
    {
    public:
        UavPositionObserver(std::shared_ptr<PositionData> data) : uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Position, nlohmann::json>(data) {}
    };


    class UavHeadingObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Heading, nlohmann::json>
    {
    public:
        UavHeadingObserver(std::shared_ptr<HeadingData> data) : uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Heading, nlohmann::json>(data) {}
    };


    class FcuHealthObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Health, nlohmann::json>
    {
    public:
        FcuHealthObserver(std::shared_ptr<FcuHealthData> data) : uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Health, nlohmann::json>(data) {}
    };


    class GpsInfoObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::GpsInfo, nlohmann::json>
    {
    public:
        GpsInfoObserver(std::shared_ptr<GpsInfoData> data) : uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::GpsInfo, nlohmann::json>(data) {}
    };


    class BatteryObserver : public uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Battery, nlohmann::json>
    {
    public:
        BatteryObserver(std::shared_ptr<BatteryData> data) : uav_ci::data_ci::DataSubscriber<mavsdk::Telemetry::Battery, nlohmann::json>(data) {}
    };









    // ############################################### 
    // ##############  Client Classes   ############## 
    // ############################################### 
    struct TelemetryData
    {
    public:
        TelemetryData()
        {
            distance_sensor_observer_p = std::make_shared<DistanceSensorObserver>(std::make_shared<DistanceSensorData>("distance_sensor"));
            flight_mode_observer_p = std::make_shared<FlightModeObserver>(std::make_shared<FlightModeData>("flight_mode"));
            attitude_euler_observer_p = std::make_shared<AttitudeEulerObserver>(std::make_shared<AttitudeEulerData>("attitude_euler"));
            home_position_observer_p = std::make_shared<HomePositionObserver>(std::make_shared<PositionData>("home_position"));
            uav_position_observer_p = std::make_shared<UavPositionObserver>(std::make_shared<PositionData>("uav_position"));
            uav_heading_observer_p = std::make_shared<UavHeadingObserver>(std::make_shared<HeadingData>("uav_heading"));
            fcu_health_observer_p = std::make_shared<FcuHealthObserver>(std::make_shared<FcuHealthData>("fcu_health"));
            gps_info_observer_p = std::make_shared<GpsInfoObserver>(std::make_shared<GpsInfoData>("gps_info"));
            battery_observer_p = std::make_shared<BatteryObserver>(std::make_shared<BatteryData>("battery_info"));
        }
        
        std::shared_ptr<DistanceSensorObserver> get_distance_sensor_observer_pointer() { return distance_sensor_observer_p; }
        std::shared_ptr<FlightModeObserver> get_flight_mode_observer_pointer() { return flight_mode_observer_p; }
        std::shared_ptr<AttitudeEulerObserver> get_attitude_euler_observer_pointer() { return attitude_euler_observer_p; }
        std::shared_ptr<HomePositionObserver> get_home_position_observer_pointer() { return home_position_observer_p; }
        std::shared_ptr<UavPositionObserver> get_uav_position_observer_pointer() { return uav_position_observer_p; }
        std::shared_ptr<UavHeadingObserver> get_uav_heading_observer_pointer() { return uav_heading_observer_p; }
        std::shared_ptr<FcuHealthObserver> get_fcu_health_observer_pointer() { return fcu_health_observer_p; }
        std::shared_ptr<GpsInfoObserver> get_gps_info_observer_pointer() { return gps_info_observer_p; }
        std::shared_ptr<BatteryObserver> get_battery_observer_pointer() { return battery_observer_p; }

        std::shared_ptr<DistanceSensorData> get_distance_sensor_data() { return std::dynamic_pointer_cast<DistanceSensorData>(distance_sensor_observer_p->get_data()); }
        std::shared_ptr<FlightModeData> get_flight_mode_data() { return std::dynamic_pointer_cast<FlightModeData>(flight_mode_observer_p->get_data()); }
        std::shared_ptr<AttitudeEulerData> get_attitude_euler_data() { return std::dynamic_pointer_cast<AttitudeEulerData>(attitude_euler_observer_p->get_data()); }
        std::shared_ptr<PositionData> get_home_position_data() { return std::dynamic_pointer_cast<PositionData>(home_position_observer_p->get_data()); }
        std::shared_ptr<PositionData> get_uav_position_data() { return std::dynamic_pointer_cast<PositionData>(uav_position_observer_p->get_data()); }
        std::shared_ptr<HeadingData> get_heading_data() { return std::dynamic_pointer_cast<HeadingData>(uav_heading_observer_p->get_data()); }
        std::shared_ptr<FcuHealthData> get_fcu_health_data() { return std::dynamic_pointer_cast<FcuHealthData>(fcu_health_observer_p->get_data()); }
        std::shared_ptr<GpsInfoData> get_gps_info_data() { return std::dynamic_pointer_cast<GpsInfoData>(gps_info_observer_p->get_data()); }
        std::shared_ptr<BatteryData> get_battery_data() { return std::dynamic_pointer_cast<BatteryData>(battery_observer_p->get_data()); }

    private:
        std::shared_ptr<DistanceSensorObserver> distance_sensor_observer_p;
        std::shared_ptr<FlightModeObserver> flight_mode_observer_p;
        std::shared_ptr<AttitudeEulerObserver> attitude_euler_observer_p;
        std::shared_ptr<HomePositionObserver> home_position_observer_p;
        std::shared_ptr<UavPositionObserver> uav_position_observer_p;
        std::shared_ptr<UavHeadingObserver> uav_heading_observer_p;
        std::shared_ptr<FcuHealthObserver> fcu_health_observer_p;
        std::shared_ptr<GpsInfoObserver> gps_info_observer_p;
        std::shared_ptr<BatteryObserver> battery_observer_p;
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

