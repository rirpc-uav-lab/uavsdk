#include <memory>

#include <uavsdk/useful_data_lib/useful_data_impl.hpp>

namespace uavsdk
{
    namespace command_manager 
    {
        namespace blackboard
        {
            class Blackboard
            {

                private:
                useful_di::UniMapStr data_storage;
            }
        }
    }
}