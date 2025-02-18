#pragma once 
#include <uavsdk/navigation/coordinates.hpp>
#include <cmath>

namespace uavsdk
{
    namespace navigation
    {
        namespace conversions
        {

            const double a = 6378137.0; // semi-major axis in meters
            const double e_sq = 0.00669437999014; // square of eccentricity

            uavsdk::navigation::coordinates::ECEFCoords wgs84ToEcef(const uavsdk::navigation::coordinates::WGS84Coords& wgs) 
            {
                uavsdk::navigation::coordinates::ECEFCoords ecef;
            
                double N = a / sqrt(1 - e_sq * sin(wgs.latitude * M_PI / 180) * sin(wgs.latitude * M_PI / 180));
                ecef.x = (N + wgs.altitude) * cos(wgs.latitude * M_PI / 180) * cos(wgs.longitude * M_PI / 180);
                ecef.y = (N + wgs.altitude) * cos(wgs.latitude * M_PI / 180) * sin(wgs.longitude * M_PI / 180);
                ecef.z = (N * (1 - e_sq) + wgs.altitude) * sin(wgs.latitude * M_PI / 180);
            
                return ecef;
            }
            
            
            uavsdk::navigation::coordinates::ENUCoords ecefToEnu(const uavsdk::navigation::coordinates::ECEFCoords& ecef, const uavsdk::navigation::coordinates::WGS84Coords& refWGS) 
            {
                // Convert reference latitude and longitude to radians
                double lat0 = refWGS.latitude * M_PI / 180;
                double lon0 = refWGS.longitude * M_PI / 180;
            
                // Reference ECEF coordinates
                uavsdk::navigation::coordinates::ECEFCoords refEcef = wgs84ToEcef(refWGS);
            
                // Compute ENU coordi   nates
                uavsdk::navigation::coordinates::ENUCoords enu;
                enu.xEast = -sin(lon0) * (ecef.x - refEcef.x) + cos(lon0) * (ecef.y - refEcef.y);
                enu.yNorth = -sin(lat0) * cos(lon0) * (ecef.x - refEcef.x) - sin(lat0) * sin(lon0) * (ecef.y - refEcef.y) + cos(lat0) * (ecef.z - refEcef.z);
                enu.zUp = cos(lat0) * cos(lon0) * (ecef.x - refEcef.x) + cos(lat0) * sin(lon0) * (ecef.y - refEcef.y) + sin(lat0) * (ecef.z - refEcef.z);
            
                return enu;
            }
        }
    }
}


