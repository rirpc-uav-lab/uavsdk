#pragma once 


namespace uavsdk
{
    namespace navigation
    {
        namespace coordinates
        {
            struct ECEFCoords
            {
                double x;
                double y;
                double z;
            };
            
            
            struct ENUCoords
            {
                double xEast;
                double yNorth;
                double zUp;
            };
            
            
            struct WGS84Coords
            {
                double latitude;
                double longitude;
                double altitude;
            };
        }
    }
}
