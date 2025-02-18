#pragma once

#include <thread>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

// #include <uavsdk/uav_control_interfaces/data_collector_implementations.hpp>
#include <uavsdk/useful_data_lib/useful_data_impl.hpp>
#include <uavsdk/useful_data_lib/useful_data_interfaces.hpp>
#include <uavsdk/utils/conversion_functions.hpp>
#include <uavsdk/utils/cpp_custom_extras.hpp>



namespace uavsdk
{
    namespace fcu_tel_collector /// fcu_telemetry_collector_mavsdk
    {
        // ############################################### 
        // ###############  Data Classes   ############### 
        // ############################################### 

        class DistanceSensorData : public useful_di::TypedDataObject<mavsdk::Telemetry::DistanceSensor>
        {
            public:
            bool initialized = false;
            DistanceSensorData(mavsdk::Telemetry::DistanceSensor msg, std::string message_name) : useful_di::TypedDataObject<mavsdk::Telemetry::DistanceSensor>(message_name)
            {
                this->___set_type();
                this->msg = msg;
                
                this->data = nlohmann::json();
                this->data["current_distance_m"] = msg.current_distance_m;
                this->data["maximum_distance_m"] = msg.maximum_distance_m;
                this->data["minimum_distance_m"] = msg.minimum_distance_m;
                this->data["orientation.pitch_deg"] = msg.orientation.pitch_deg;
                this->data["orientation.roll_deg"] = msg.orientation.roll_deg;
                this->data["orientation.yaw_deg"] = msg.orientation.yaw_deg;
                this->data["initialized"] = true;
                this->initialized = true;
            }

            DistanceSensorData(std::string message_name) : useful_di::TypedDataObject<mavsdk::Telemetry::DistanceSensor>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::DistanceSensor();
                
                this->data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<DistanceSensorData>();
            }
        };


        class FlightModeData : public useful_di::TypedDataObject<mavsdk::Telemetry::FlightMode>
        {
            public:
            bool initialized = false;
            FlightModeData(mavsdk::Telemetry::FlightMode msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::FlightMode>(message_name)
            {
                this->___set_type();
                this->msg = msg;
                
                data = nlohmann::json();
                data["flight_mode"] = utils::conversions::mavsdk_ext::convert_flight_mode_to_str(msg);
                this->data["initialized"] = true;
                this->initialized = true;
            }
            
            FlightModeData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::FlightMode>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::FlightMode();

                this->data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<FlightModeData>();
            }
        };


        class AttitudeEulerData : public useful_di::TypedDataObject<mavsdk::Telemetry::EulerAngle>
        {
            public:
            bool initialized = false;
            AttitudeEulerData(mavsdk::Telemetry::EulerAngle msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::EulerAngle>(message_name)
            {
                this->___set_type();
                this->msg = msg;

                data = nlohmann::json();
                data["pitch_deg"] = msg.pitch_deg;
                data["roll_deg"] = msg.roll_deg;
                data["yaw_deg"] = msg.yaw_deg;
                this->data["initialized"] = true;
                this->initialized = true;
            }

            AttitudeEulerData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::EulerAngle>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::EulerAngle(); 
                data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<AttitudeEulerData>();
            }
        };


        class PositionData : public useful_di::TypedDataObject<mavsdk::Telemetry::Position>
        {
            public:
            bool initialized = false;
            PositionData(mavsdk::Telemetry::Position msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::Position>(message_name)
            {
                this->___set_type();
                this->msg = msg;
                
                data = nlohmann::json();
                data["absolute_altitude_m"] = msg.absolute_altitude_m;
                data["latitude_deg"] = msg.latitude_deg;
                data["longitude_deg"] = msg.longitude_deg;
                data["relative_altitude_m"] = msg.relative_altitude_m;
                this->data["initialized"] = true;
                this->initialized = true;
            }

            PositionData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::Position>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::Position();
                
                data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<PositionData>();
            }
        };


        class HeadingData : public useful_di::TypedDataObject<mavsdk::Telemetry::Heading>
        {
            public:
            bool initialized = false;
            HeadingData(mavsdk::Telemetry::Heading msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::Heading>(message_name)
            {
                this->___set_type();
                this->msg = msg;
                
                data = nlohmann::json();
                data["heading_deg"] = msg.heading_deg;
                this->data["initialized"] = true;
                this->initialized = true;
            }

            HeadingData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::Heading>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::Heading();
                
                data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<HeadingData>();
            }
        };


        class FcuHealthData : public useful_di::TypedDataObject<mavsdk::Telemetry::Health>
        {
            public:
            bool initialized = false;
            FcuHealthData(mavsdk::Telemetry::Health msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::Health>(message_name)
            {
                this->___set_type();
                this->msg = msg;
                
                data = nlohmann::json();
                data["is_accelerometer_calibration_ok"] = msg.is_accelerometer_calibration_ok;
                data["is_armable"] = msg.is_armable;
                data["is_global_position_ok"] = msg.is_global_position_ok;
                data["is_gyrometer_calibration_ok"] = msg.is_gyrometer_calibration_ok;
                data["is_home_position_ok"] = msg.is_home_position_ok;
                data["is_local_position_ok"] = msg.is_local_position_ok;
                data["is_magnetometer_calibration_ok"] = msg.is_magnetometer_calibration_ok;
                this->data["initialized"] = true;
                this->initialized = true;
            }

            FcuHealthData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::Health>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::Health();
                
                data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<FcuHealthData>();
            }
        };


        class GpsInfoData : public useful_di::TypedDataObject<mavsdk::Telemetry::GpsInfo>
        {
            public:
            bool initialized = false;
            GpsInfoData(mavsdk::Telemetry::GpsInfo msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::GpsInfo>(message_name)
            {
                this->___set_type();
                this->msg = msg;
                
                data = nlohmann::json();
                data["fix_type"] = utils::conversions::mavsdk_ext::convert_fix_type_to_str(msg.fix_type);
                data["num_satellites"] = msg.num_satellites;
                this->data["initialized"] = true;
                this->initialized = true;
            }
            
            GpsInfoData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::GpsInfo>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::GpsInfo();
                
                data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<GpsInfoData>();
            }
        };


        class BatteryData : public useful_di::TypedDataObject<mavsdk::Telemetry::Battery>
        {
            public:
            bool initialized = false;
            BatteryData(mavsdk::Telemetry::Battery msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::Battery>(message_name)
            {
                this->___set_type();
                this->msg = msg;
                
                data = nlohmann::json();
                data["capacity_consumed_ah"] = msg.capacity_consumed_ah;
                data["current_battery_a"] = msg.current_battery_a;
                data["id"] = msg.id;
                data["remaining_percent"] = msg.remaining_percent;
                data["temperature_degc"] = msg.temperature_degc;
                data["voltage_v"] = msg.voltage_v;
                this->data["initialized"] = true;
                this->initialized = true;
            }

            BatteryData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::Battery>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::Battery();
                
                data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<BatteryData>();
            }
        };


        class LandedStateData : public useful_di::TypedDataObject<mavsdk::Telemetry::LandedState>
        {
            public:
            bool initialized = false;
            LandedStateData(mavsdk::Telemetry::LandedState msg, std::string message_name) : TypedDataObject<mavsdk::Telemetry::LandedState>(message_name)
            {
                this->___set_type();
                this->msg = msg;
                
                data = nlohmann::json();
                if (msg == mavsdk::Telemetry::LandedState::InAir)
                {
                    data["landed_state"] = "InAir";
                }
                else if (msg == mavsdk::Telemetry::LandedState::Landing)
                {
                    data["landed_state"] = "Landing";
                }
                else if (msg == mavsdk::Telemetry::LandedState::OnGround)
                {
                    data["landed_state"] = "OnGround";
                }
                else if (msg == mavsdk::Telemetry::LandedState::Unknown)
                {
                    data["landed_state"] = "Unknown";
                }
                else if (msg == mavsdk::Telemetry::LandedState::TakingOff)
                {
                    data["landed_state"] = "TakingOff";
                }
                else
                {
                    throw std::runtime_error("Unknown LandedState");
                }
                
                
                this->data["initialized"] = true;

                this->initialized = true;
            }

            LandedStateData(std::string message_name) : TypedDataObject<mavsdk::Telemetry::LandedState>(message_name)
            {
                this->___set_type();
                this->msg = mavsdk::Telemetry::LandedState();
                
                data = nlohmann::json();
                this->data["initialized"] = false;
            }


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<LandedStateData>();
            }
        };








        // ############################################### 
        // ##########  Data Collector Classes   ########## 
        // ############################################### 
        class DistanceSensorCollector : public useful_di::SingleObserverDataCollector<DistanceSensorData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::DistanceSensorHandle handle;
        public:
            DistanceSensorCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<useful_di::DataObserverInterface<DistanceSensorData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_distance_sensor" << std::endl;
                handle = telemetry->subscribe_distance_sensor([this](mavsdk::Telemetry::DistanceSensor data)
                {
                    // std::cout << "subscribe_distance_sensor" << std::endl;
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


        class FlightModeCollector : public useful_di::SingleObserverDataCollector<FlightModeData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::FlightModeHandle handle;
        public:
            FlightModeCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<useful_di::DataObserverInterface<FlightModeData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_flight_mode" << std::endl;
                handle = telemetry->subscribe_flight_mode([this](mavsdk::Telemetry::FlightMode data)
                {
                    // std::cout << "subscribe_flight_mode" << std::endl;
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


        class AttitudeEulerCollector : public useful_di::SingleObserverDataCollector<AttitudeEulerData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::AttitudeEulerHandle handle;
            std::shared_ptr<AttitudeEulerData> data_p;
        public:
            AttitudeEulerCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<useful_di::DataObserverInterface<AttitudeEulerData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);
                // std::cout << "subscribe_attitude_euler" << std::endl;
                handle = telemetry->subscribe_attitude_euler([this](mavsdk::Telemetry::EulerAngle data)
                {
                    // std::cout << "subscribe_attitude_euler" << std::endl;
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
        

        class HomePositionCollector : public useful_di::SingleObserverDataCollector<PositionData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::HomeHandle handle;
        public:
            HomePositionCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<useful_di::DataObserverInterface<PositionData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_home" << std::endl;
                handle = telemetry->subscribe_home([this](mavsdk::Telemetry::Position data)
                {
                    // std::cout << "subscribe_home" << std::endl;
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


        class UavPositionCollector : public useful_di::SingleObserverDataCollector<PositionData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::HomeHandle handle;
        public:
            UavPositionCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<useful_di::DataObserverInterface<PositionData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_position" << std::endl;
                handle = telemetry->subscribe_position([this](mavsdk::Telemetry::Position data)
                {
                    // std::cout << "subscribe_position" << std::endl;
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


        class UavHeadingCollector : public useful_di::SingleObserverDataCollector<HeadingData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::HeadingHandle handle;
        public:
            UavHeadingCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<useful_di::DataObserverInterface<HeadingData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_heading" << std::endl;
                handle = telemetry->subscribe_heading([this](mavsdk::Telemetry::Heading data)
                {
                    // std::cout << "subscribe_heading" << std::endl;
                    std::thread([this, data]()
                    {
                        this->update_data(std::make_shared<HeadingData>(data, "uav_heading"));
                    }).detach();
                });
            }


            ~UavHeadingCollector()
            {
                telemetry->unsubscribe_heading(handle);
            }
        };


        class FcuHealthCollector : public useful_di::SingleObserverDataCollector<FcuHealthData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::HealthHandle handle;
        public:
            FcuHealthCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, std::shared_ptr<useful_di::DataObserverInterface<FcuHealthData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_health" << std::endl;
                handle = telemetry->subscribe_health([this](mavsdk::Telemetry::Health data)
                {
                    // std::cout << "subscribe_health" << std::endl;
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


        class GpsInfoCollector : public useful_di::SingleObserverDataCollector<GpsInfoData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::GpsInfoHandle handle;
        public:
            GpsInfoCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, 
                            std::shared_ptr<useful_di::DataObserverInterface<GpsInfoData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_gps_info" << std::endl;
                handle = telemetry->subscribe_gps_info([this](mavsdk::Telemetry::GpsInfo data)
                {
                    // std::cout << "subscribe_gps_info" << std::endl;
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


        class BatteryCollector : public useful_di::SingleObserverDataCollector<BatteryData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::BatteryHandle handle;
        public:
            BatteryCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, 
                            std::shared_ptr<useful_di::DataObserverInterface<BatteryData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_battery" << std::endl;
                handle = telemetry->subscribe_battery([this](mavsdk::Telemetry::Battery data)
                {
                    // std::cout << "subscribe_battery" << std::endl;
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


        class LandedStateCollector : public useful_di::SingleObserverDataCollector<LandedStateData>
        {
        private:
            std::shared_ptr<mavsdk::Telemetry> telemetry;
            mavsdk::Telemetry::LandedStateHandle handle;
        public:
            LandedStateCollector(std::shared_ptr<mavsdk::Telemetry> telemetry_p, 
                            std::shared_ptr<useful_di::DataObserverInterface<LandedStateData>> observer)
            {
                this->telemetry = telemetry_p;
                this->attach_observer(observer);

                // std::cout << "subscribe_battery" << std::endl;
                handle = telemetry->subscribe_landed_state([this](mavsdk::Telemetry::LandedState data)
                {
                    // std::cout << "subscribe_battery" << std::endl;
                    std::thread([this, data]()
                    {
                        this->update_data(std::make_shared<LandedStateData>(data, "landed_state"));
                    }).detach();
                });
            }


            ~LandedStateCollector()
            {
                telemetry->unsubscribe_landed_state(handle);
            }
        };













        // ############################################### 
        // ###########  Data Observer Classes   ########## 
        // ############################################### 
        class DistanceSensorObserver : public useful_di::DataSubscriber<DistanceSensorData>
        {
        public:
            DistanceSensorObserver(std::shared_ptr<DistanceSensorData> data) : useful_di::DataSubscriber<DistanceSensorData>(data) {}
        };


        class FlightModeObserver : public useful_di::DataSubscriber<FlightModeData>
        {
        public:
            FlightModeObserver(std::shared_ptr<FlightModeData> data) :  useful_di::DataSubscriber<FlightModeData>(data) {}
        };


        class AttitudeEulerObserver : public useful_di::DataSubscriber<AttitudeEulerData>
        {
        public:
            AttitudeEulerObserver(std::shared_ptr<AttitudeEulerData> data) : useful_di::DataSubscriber<AttitudeEulerData>(data) {}
        };


        class HomePositionObserver : public useful_di::DataSubscriber<PositionData>
        {
        public:
            HomePositionObserver(std::shared_ptr<PositionData> data) : useful_di::DataSubscriber<PositionData>(data) {}
        };


        class UavPositionObserver : public useful_di::DataSubscriber<PositionData>
        {
        public:
            UavPositionObserver(std::shared_ptr<PositionData> data) : useful_di::DataSubscriber<PositionData>(data) {}
        };


        class UavHeadingObserver : public useful_di::DataSubscriber<HeadingData>
        {
        public:
            UavHeadingObserver(std::shared_ptr<HeadingData> data) : useful_di::DataSubscriber<HeadingData>(data) {}
        };


        class FcuHealthObserver : public useful_di::DataSubscriber<FcuHealthData>
        {
        public:
            FcuHealthObserver(std::shared_ptr<FcuHealthData> data) : useful_di::DataSubscriber<FcuHealthData>(data) {}
        };


        class GpsInfoObserver : public useful_di::DataSubscriber<GpsInfoData>
        {
        public:
            GpsInfoObserver(std::shared_ptr<GpsInfoData> data) : useful_di::DataSubscriber<GpsInfoData>(data) {}
        };


        class BatteryObserver : public useful_di::DataSubscriber<BatteryData>
        {
        public:
            BatteryObserver(std::shared_ptr<BatteryData> data) : useful_di::DataSubscriber<BatteryData>(data) {}
        };


        class LandedStateObserver : public useful_di::DataSubscriber<LandedStateData>
        {
        public:
            LandedStateObserver(std::shared_ptr<LandedStateData> data) : useful_di::DataSubscriber<LandedStateData>(data) {}
        };




        enum class TelemetryDataTypeId
        {
            DistanceSensorData,
            FlightModeData,
            AttitudeEulerData,
            HomePositionData,
            UavPositionData,
            HeadingData,
            FcuHealthData,
            GpsInfoData,
            BatteryData,
            LandedStateData,
        };
        



        class IdFactoryTelemetryData : public useful_di::IdFactoryInterface<TelemetryDataTypeId>
        {
            public: 
                
                TelemetryDataTypeId get_id(std::shared_ptr<useful_di::TypeInterface> data) override
                {
                    TelemetryDataTypeId id;
                    
                    if (data->___get_type() == utils::cppext::get_type<DistanceSensorData>())
                    {
                        id = TelemetryDataTypeId::DistanceSensorData;
                        return id;
                    }
                    else if (data->___get_type() == utils::cppext::get_type<FlightModeData>())
                    {
                        id = TelemetryDataTypeId::FlightModeData;
                        return id;
                    }
                    else if (data->___get_type() == utils::cppext::get_type<AttitudeEulerData>())
                    {
                        id = TelemetryDataTypeId::AttitudeEulerData;
                        return id;
                    }
                    else if (data->___get_type() == utils::cppext::get_type<PositionData>())
                    {
                        auto _data = std::dynamic_pointer_cast<PositionData>(data);
                        // auto _data = std::dynamic_pointer_cast<useful_di::DataObjectJson<mavsdk::Telemetry::Position>>(data);
                        std::string data_type = _data->get_name();
                        if (data_type == "home_position") id = TelemetryDataTypeId::HomePositionData;
                        else if (data_type == "uav_position") id = TelemetryDataTypeId::UavPositionData;
                        else    
                        {
                            throw std::runtime_error("Unknown data meaning");
                        }
                        return id;
                    }
                    else if (data->___get_type() == utils::cppext::get_type<HeadingData>())
                    {
                        id = TelemetryDataTypeId::HeadingData;
                        return id;
                    }
                    else if (data->___get_type() == utils::cppext::get_type<FcuHealthData>())
                    {
                        id = TelemetryDataTypeId::FcuHealthData;
                        return id;
                    }
                    else if (data->___get_type() == utils::cppext::get_type<GpsInfoData>())
                    {
                        id = TelemetryDataTypeId::GpsInfoData;
                        return id;
                    }
                    else if (data->___get_type() == utils::cppext::get_type<BatteryData>())
                    {
                        id = TelemetryDataTypeId::BatteryData;
                        return id;
                    }
                    else if (data->___get_type() == utils::cppext::get_type<LandedStateData>())
                    {
                        id = TelemetryDataTypeId::LandedStateData;
                        return id;
                    }
                    else throw std::runtime_error("Unknown data type. Current one is:  " + data->___get_type());
                }
        };




        // ############################################### 
        // ##############  Client Classes   ############## 
        // ############################################### 
        // struct TelemetryDataComposite : useful_di::DataCompositeJson<>
        // class TelemetryDataComposite : public useful_di::DataCompositeInterface<nlohmann::json, TelemetryDataTypeId, useful_di::RegistryDataStorage<TelemetryDataTypeId, IdFactoryTelemetryData, nlohmann::json>>
        class TelemetryDataComposite : public useful_di::DataCompositeInterface<nlohmann::json, std::string, useful_di::UniMapStr>
        {
        public:
            TelemetryDataComposite(std::shared_ptr<mavsdk::Telemetry>& telemetry_p) 
            {
                this->___set_type();
                telemetry = telemetry_p;

                decorated_composite = std::make_shared<useful_di::DataCompositeJson>();

                distance_sensor_observer_p = std::make_shared<DistanceSensorObserver>(std::make_shared<DistanceSensorData>("distance_sensor"));
                flight_mode_observer_p = std::make_shared<FlightModeObserver>(std::make_shared<FlightModeData>("flight_mode"));
                attitude_euler_observer_p = std::make_shared<AttitudeEulerObserver>(std::make_shared<AttitudeEulerData>("attitude_euler"));
                home_position_observer_p = std::make_shared<HomePositionObserver>(std::make_shared<PositionData>("home_position"));
                uav_position_observer_p = std::make_shared<UavPositionObserver>(std::make_shared<PositionData>("uav_position"));
                uav_heading_observer_p = std::make_shared<UavHeadingObserver>(std::make_shared<HeadingData>("uav_heading"));
                fcu_health_observer_p = std::make_shared<FcuHealthObserver>(std::make_shared<FcuHealthData>("fcu_health"));
                gps_info_observer_p = std::make_shared<GpsInfoObserver>(std::make_shared<GpsInfoData>("gps_info"));
                battery_observer_p = std::make_shared<BatteryObserver>(std::make_shared<BatteryData>("battery_info"));
                landed_state_observer_p = std::make_shared<LandedStateObserver>(std::make_shared<LandedStateData>("landed_state"));
            
                distance_sensorcollector_p = std::make_shared<DistanceSensorCollector>(telemetry, this->distance_sensor_observer_p);
                flight_modecollector_p = std::make_shared<FlightModeCollector>(telemetry, this->flight_mode_observer_p);
                attitude_eulercollector_p = std::make_shared<AttitudeEulerCollector>(telemetry, this->attitude_euler_observer_p);
                home_positioncollector_p = std::make_shared<HomePositionCollector>(telemetry, this->home_position_observer_p);
                uav_positioncollector_p = std::make_shared<UavPositionCollector>(telemetry, this->uav_position_observer_p);
                uav_headingcollector_p = std::make_shared<UavHeadingCollector>(telemetry, this->uav_heading_observer_p);
                fcu_healthcollector_p = std::make_shared<FcuHealthCollector>(telemetry, this->fcu_health_observer_p);
                gps_infocollector_p = std::make_shared<GpsInfoCollector>(telemetry, this->gps_info_observer_p);
                batterycollector_p = std::make_shared<BatteryCollector>(telemetry, this->battery_observer_p);
                landed_state_collector_p = std::make_shared<LandedStateCollector>(telemetry, this->landed_state_observer_p);
            }
            

            void add_data(std::shared_ptr<TypeInterface> data) override
            {
                this->decorated_composite->add_data(data);
            }

            // bool all_data_initialized()
            // {
            //     bool init = true; 
            //     std::shared_ptr<useful_di::UniMapStr> storage = std::dynamic_pointer_cast<useful_di::UniMapStr>(this->decorated_composite->get_msg());
            //     int i = 0;

            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<DistanceSensorData>(this->distance_sensor_observer_p->get_data())->get_name(), this->distance_sensor_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<FlightModeData>(this->flight_mode_observer_p->get_data())->get_name(), this->flight_mode_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<AttitudeEulerData>(this->attitude_euler_observer_p->get_data())->get_name(), this->attitude_euler_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<PositionData>(this->home_position_observer_p->get_data())->get_name(), this->home_position_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<PositionData>(this->uav_position_observer_p->get_data())->get_name(), this->uav_position_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<HeadingData>(this->uav_heading_observer_p->get_data())->get_name(), this->uav_heading_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<FcuHealthData>(this->fcu_health_observer_p->get_data())->get_name(), this->fcu_health_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<GpsInfoData>(this->gps_info_observer_p->get_data())->get_name(), this->gps_info_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<BatteryData>(this->battery_observer_p->get_data())->get_name(), this->battery_observer_p->get_data());
            //     this->decorated_composite->add_data(std::dynamic_pointer_cast<LandedStateData>(this->landed_state_observer_p->get_data())->get_name(), this->landed_state_observer_p->get_data());

            //     for (auto it = storage->begin(); it != storage->end(); it++)
            //     {
            //         bool _init = std::dynamic_pointer_cast<useful_di::UniversalDataInterface<nlohmann::json>>(it->second)->get_data()["initialized"];
            //         init = init and _init;
            //         std::cout << "\nall_data_initialized: " << it->second->___get_type() << " = " << _init << "\n";
            //     }

            //     return init;
            // }

            void add_data(std::string key, std::shared_ptr<TypeInterface> data)
            {
                this->decorated_composite->add_data(key, data);
            }

            bool health_all_ok()
            {
                mavsdk::Telemetry::Health health = std::dynamic_pointer_cast<FcuHealthData>(this->get_msg()->at("fcu_health"))->get_msg();
                bool all_ok = health.is_accelerometer_calibration_ok and health.is_armable and health.is_global_position_ok and health.is_gyrometer_calibration_ok and health.is_home_position_ok and health.is_local_position_ok and health.is_magnetometer_calibration_ok;

                // std::cout << "health.is_accelerometer_calibration_ok: " << health.is_accelerometer_calibration_ok << "\n"; 
                // std::cout << "health.is_armable: " << health.is_armable << "\n"; 
                // std::cout << "health.is_global_position_ok: " << health.is_global_position_ok << "\n"; 
                // std::cout << "health.is_gyrometer_calibration_ok: " << health.is_gyrometer_calibration_ok << "\n"; 
                // std::cout << "health.is_home_position_ok: " << health.is_home_position_ok << "\n"; 
                // std::cout << "health.is_local_position_ok: " << health.is_local_position_ok << "\n"; 
                // std::cout << "health.is_magnetometer_calibration_ok: " << health.is_magnetometer_calibration_ok << "\n"; 
                
                return all_ok;
            }


        private:
            std::shared_ptr<useful_di::DataCompositeJson> decorated_composite;

            std::shared_ptr<DistanceSensorObserver> distance_sensor_observer_p;
            std::shared_ptr<FlightModeObserver> flight_mode_observer_p;
            std::shared_ptr<AttitudeEulerObserver> attitude_euler_observer_p;
            std::shared_ptr<HomePositionObserver> home_position_observer_p;
            std::shared_ptr<UavPositionObserver> uav_position_observer_p;
            std::shared_ptr<UavHeadingObserver> uav_heading_observer_p;
            std::shared_ptr<FcuHealthObserver> fcu_health_observer_p;
            std::shared_ptr<GpsInfoObserver> gps_info_observer_p;
            std::shared_ptr<BatteryObserver> battery_observer_p;
            std::shared_ptr<LandedStateObserver> landed_state_observer_p;

            std::shared_ptr<DistanceSensorCollector> distance_sensorcollector_p;
            std::shared_ptr<FlightModeCollector> flight_modecollector_p;
            std::shared_ptr<AttitudeEulerCollector> attitude_eulercollector_p;
            std::shared_ptr<HomePositionCollector> home_positioncollector_p;
            std::shared_ptr<UavPositionCollector> uav_positioncollector_p;
            std::shared_ptr<UavHeadingCollector> uav_headingcollector_p;
            std::shared_ptr<FcuHealthCollector> fcu_healthcollector_p;
            std::shared_ptr<GpsInfoCollector> gps_infocollector_p;
            std::shared_ptr<BatteryCollector> batterycollector_p;
            std::shared_ptr<LandedStateCollector> landed_state_collector_p;

            std::shared_ptr<mavsdk::Telemetry> telemetry;


            void ___set_type() override
            {
                this->___type = utils::cppext::get_type<TelemetryDataComposite>();
            }


            nlohmann::json _get_data() override
            {
                // while (not this->all_data_initialized())
                // {
                //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
                //     std::cout << "\n\n\nDATA NOT READY!!!\n\n\n";
                //     std::cout << "ready: " << this->all_data_initialized() << "\n";
                // }
                // std::cout << "TelemetryDataComposite: " << std::dynamic_pointer_cast<DistanceSensorData>(this->distance_sensor_observer_p->get_data())->get_name() << "\n";
                this->decorated_composite->add_data(std::dynamic_pointer_cast<DistanceSensorData>(this->distance_sensor_observer_p->get_data())->get_name(), this->distance_sensor_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<FlightModeData>(this->flight_mode_observer_p->get_data())->get_name(), this->flight_mode_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<AttitudeEulerData>(this->attitude_euler_observer_p->get_data())->get_name(), this->attitude_euler_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<PositionData>(this->home_position_observer_p->get_data())->get_name(), this->home_position_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<PositionData>(this->uav_position_observer_p->get_data())->get_name(), this->uav_position_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<HeadingData>(this->uav_heading_observer_p->get_data())->get_name(), this->uav_heading_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<FcuHealthData>(this->fcu_health_observer_p->get_data())->get_name(), this->fcu_health_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<GpsInfoData>(this->gps_info_observer_p->get_data())->get_name(), this->gps_info_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<BatteryData>(this->battery_observer_p->get_data())->get_name(), this->battery_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<LandedStateData>(this->landed_state_observer_p->get_data())->get_name(), this->landed_state_observer_p->get_data());
                return this->decorated_composite->get_data();
            }


            std::shared_ptr<useful_di::DataStorageInterface<std::string>> _get_msg() override
            { 
                // while (not this->all_data_initialized())
                // {
                //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
                //     std::cout << "\n\n\nDATA NOT READY!!!\n\n\n";
                //     std::cout << "ready: " << this->all_data_initialized() << "\n";
                // }
                // std::cout << "TelemetryDataComposite: " << std::dynamic_pointer_cast<DistanceSensorData>(this->distance_sensor_observer_p->get_data())->get_name() << "\n";
                this->decorated_composite->add_data(std::dynamic_pointer_cast<DistanceSensorData>(this->distance_sensor_observer_p->get_data())->get_name(), this->distance_sensor_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<FlightModeData>(this->flight_mode_observer_p->get_data())->get_name(), this->flight_mode_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<AttitudeEulerData>(this->attitude_euler_observer_p->get_data())->get_name(), this->attitude_euler_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<PositionData>(this->home_position_observer_p->get_data())->get_name(), this->home_position_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<PositionData>(this->uav_position_observer_p->get_data())->get_name(), this->uav_position_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<HeadingData>(this->uav_heading_observer_p->get_data())->get_name(), this->uav_heading_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<FcuHealthData>(this->fcu_health_observer_p->get_data())->get_name(), this->fcu_health_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<GpsInfoData>(this->gps_info_observer_p->get_data())->get_name(), this->gps_info_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<BatteryData>(this->battery_observer_p->get_data())->get_name(), this->battery_observer_p->get_data());
                this->decorated_composite->add_data(std::dynamic_pointer_cast<LandedStateData>(this->landed_state_observer_p->get_data())->get_name(), this->landed_state_observer_p->get_data());

                return std::dynamic_pointer_cast<useful_di::UniMapStr>(this->decorated_composite->get_msg());
            }
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
        //     TelemetryDataComposite telemetry_data;

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

};