#pragma once

#include <string>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

namespace utils
{
    namespace conversions
    {
        namespace mavsdk_ext
        {

            std::string convert_flight_mode_to_str(mavsdk::Telemetry::FlightMode& flight_mode)
            {
                if (flight_mode == mavsdk::Telemetry::FlightMode::Land)
                {
                    return "Unknown";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Ready)
                {
                    return "Ready";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Takeoff)
                {
                    return "Takeoff";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Hold)
                {
                    return "Hold";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Mission)
                {
                    return "Mission";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::ReturnToLaunch)
                {
                    return "ReturnToLaunch";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Land)
                {
                    return "Land";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Offboard)
                {
                    return "Offboard";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::FollowMe)
                {
                    return "FollowMe";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Manual)
                {
                    return "Manual";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Altctl)
                {
                    return "Altctl";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Posctl)
                {
                    return "Posctl";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Acro)
                {
                    return "Acro";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Stabilized)
                {
                    return "Stabilized";
                }
                else if (flight_mode == mavsdk::Telemetry::FlightMode::Rattitude)
                {
                    return "Rattitude";
                }
                else 
                {
                    return "Unknown";
                }
            }

            std::string convert_fix_type_to_str(mavsdk::Telemetry::FixType& fix_type)
            {
                if (fix_type == mavsdk::Telemetry::FixType::Fix2D) return "Fix2D";
                else if (fix_type == mavsdk::Telemetry::FixType::Fix3D) return "Fix3D";
                else if (fix_type == mavsdk::Telemetry::FixType::FixDgps) return "FixDgps";
                else if (fix_type == mavsdk::Telemetry::FixType::NoFix) return "NoFix";
                else if (fix_type == mavsdk::Telemetry::FixType::NoGps) return "NoGps";
                else if (fix_type == mavsdk::Telemetry::FixType::RtkFixed) return "RtkFixed";
                else if (fix_type == mavsdk::Telemetry::FixType::RtkFloat) return "RtkFloat";
                else { return "Unknown"; }
            }
        };
    };
};